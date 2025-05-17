#include "minishell.h"

int	is_redirect(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND
		|| type == TOKEN_REDIRECT_HEREDOC);
}

t_redirect_type	token_to_redirect(t_token_type type)
{
	if (type == TOKEN_REDIRECT_IN)
		return (REDIR_IN);
	if (type == TOKEN_REDIRECT_OUT)
		return (REDIR_OUT);
	if (type == TOKEN_REDIRECT_APPEND)
		return (REDIR_APPEND);
	return (REDIR_HEREDOC);
}

t_word	*copy_word(t_word *src)
{
	t_word		*dst;
	t_list		*segments;
	t_list		*tmp;
	t_segment	*src_seg;
	t_segment	*dst_seg;

	dst = ft_calloc(1, sizeof(t_word));
	segments = NULL;
	tmp = src->segments;
	while (tmp)
	{
		src_seg = tmp->content;
		dst_seg = ft_calloc(1, sizeof(t_segment));
		dst_seg->type = src_seg->type;
		dst_seg->value = ft_strdup(src_seg->value);
		ft_lstadd_back(&segments, ft_lstnew(dst_seg));
		tmp = tmp->next;
	}
	dst->segments = segments;
	return (dst);
}

void	free_segment(void *seg_ptr)
{
	t_segment	*seg;

	seg = seg_ptr;
	free(seg->value);
	free(seg);
}

void	free_word(void *word_ptr)
{
	t_word	*w;

	w = word_ptr;
	ft_lstclear(&w->segments, free_segment);
	free(w);
}

void	free_redirect(void *redir_ptr)
{
	t_redirect	*r;

	r = redir_ptr;
	free_word(r->filename);
	free(r);
}

void	free_command(void *cmd_ptr)
{
	t_command	*cmd;

	cmd = cmd_ptr;
	ft_lstclear(&cmd->arguments, free_word);
	ft_lstclear(&cmd->redirects, free_redirect);
	free(cmd);
}

int	handle_redirect(t_list **tokens, t_command *cmd)
{
	t_token		*token;
	t_redirect	*redir;
	t_list		*file_node;
	t_token		*file_tok;

	token = (*tokens)->content;
	redir = ft_calloc(1, sizeof(*redir));
	if (!redir)
		return 0;
	*tokens = (*tokens)->next;
	redir->type = token_to_redirect(token->type);
	if (!*tokens
	 || ((t_token *)(*tokens)->content)->type != TOKEN_WORD)
	{
		free(redir);
		return 0;
	}
	file_node = *tokens;
	file_tok = file_node->content;
	redir->filename = copy_word(file_tok->word);
	if (!redir->filename)
	{
		free(redir);
		return 0;
	}
	ft_lstadd_back(&cmd->redirects, ft_lstnew(redir));
	*tokens = file_node->next;
	free(file_node);
	return 1;
}

int	process_redirect(t_list **tokens, t_command *cmd, t_list *head)
{
	if (!handle_redirect(tokens, cmd))
	{
		free(head);
		free_command(cmd);
		return (0);
	}
	free(head);
	return (1);
}

int	process_word(t_list **tokens, t_command *cmd, t_list *head)
{
	t_token	*token;
	t_word	*arg;

	token = head->content;
	arg = copy_word(token->word);
	if (!arg)
	{
		free(head);
		free_command(cmd);
		return (0);
	}
	ft_lstadd_back(&cmd->arguments, ft_lstnew(arg));
	*tokens = head->next;
	free(head);
	return (1);
}

t_command *parse_command(t_list **tokens)
{
	t_command *cmd = ft_calloc(1, sizeof(t_command));
	if (!cmd)
		return NULL;

	while (*tokens) {
		t_list *head = *tokens;
		t_token *token = head->content;

		if (is_redirect(token->type)) {
			if (!process_redirect(tokens, cmd, head))
				return NULL;
		}
		else if (token->type == TOKEN_WORD) {
			if (!process_word(tokens, cmd, head))
				return NULL;
		}
		else {
			free(head);
			free_command(cmd);
			return NULL;
		}
	}
	return cmd;
}

void	clear_token_list(void *content)
{
	t_list *lst = content;
	ft_lstclear(&lst, free_token);
}

void add_token_to_current(t_list **current, t_token *token)
{
	t_list *new_node = ft_lstnew(token);
	if (!*current)
		*current = new_node;
	else
		ft_lstadd_back(current, new_node);
}

int finalize_current_command(t_list **cmds, t_list **current)
{
	if (!*current)
		return -1;
	ft_lstadd_back(cmds, ft_lstnew(*current));
	*current = NULL;
	return 0;
}

t_list *clear_on_error(t_list **current, t_list **cmds)
{
	if (*current)
		ft_lstclear(current, NULL);
	if (*cmds)
		ft_lstclear(cmds, NULL);
	return NULL;
}

t_list *split_commands(t_list *tokens)
{
	t_list *cmds = NULL;
	t_list *current = NULL;

	while (tokens)
	{
		t_token *token = tokens->content;

		if (token->type == TOKEN_PIPE)
		{
			if (!current)
            {
                fprintf(stderr,
                        "minishell: syntax error near unexpected token `|'\n");
                return clear_on_error(&current, &cmds);
            }
			if (finalize_current_command(&cmds, &current) == -1)
				return clear_on_error(&current, &cmds);
		}
		else
			add_token_to_current(&current, token);
		tokens = tokens->next;
	}
	if (current)
		ft_lstadd_back(&cmds, ft_lstnew(current));
	return cmds;
}

t_pipeline *parse(t_list *tokens)
{
	t_pipeline *pipeline = ft_calloc(1, sizeof(t_pipeline));
	t_list *cmd_groups = split_commands(tokens);
	
	if (!cmd_groups)
	{
		free(pipeline);
		return NULL;
	}
	t_list *curr_group = cmd_groups;
	while (curr_group)
	{
		t_list **cmd_tokens = (t_list **)&curr_group->content;
		t_command *cmd = parse_command(cmd_tokens);
		
		if (!cmd || *cmd_tokens != NULL) {
			ft_lstclear(&pipeline->commands, free_command);
			free(pipeline);
			ft_lstclear(&cmd_groups, clear_token_list);
			return NULL;
		}
		ft_lstadd_back(&pipeline->commands, ft_lstnew(cmd));
		curr_group = curr_group->next;
	}
	ft_lstclear(&cmd_groups, clear_token_list);
	return pipeline;
}

const char *redirect_type_str(t_redirect_type type)
{
	const char *names[] = {
		[REDIR_IN] = "INPUT",
		[REDIR_OUT] = "OUTPUT",
		[REDIR_APPEND] = "APPEND",
		[REDIR_HEREDOC] = "HEREDOC"
	};
	return names[type];
}

void free_pipeline(t_pipeline *pipeline)
{
	ft_lstclear(&pipeline->commands, free_command);
	free(pipeline);
}
