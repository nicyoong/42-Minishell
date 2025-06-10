/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirect.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 00:14:25 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/11 01:24:58 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirect(t_token_type type)
{
	return (type == TOKEN_REDIRECT_IN
		|| type == TOKEN_REDIRECT_OUT
		|| type == TOKEN_REDIRECT_APPEND
		|| type == TOKEN_REDIRECT_HEREDOC
		|| type == TOKEN_HERESTRING);
}

t_redirect_type	token_to_redirect(t_token_type type)
{
	if (type == TOKEN_REDIRECT_IN)
		return (REDIR_IN);
	if (type == TOKEN_REDIRECT_OUT)
		return (REDIR_OUT);
	if (type == TOKEN_REDIRECT_APPEND)
		return (REDIR_APPEND);
	if (type == TOKEN_HERESTRING)
		return (REDIR_HERESTRING);
	return (REDIR_HEREDOC);
}

int	init_redirect(t_list **tokens, t_redirect **redir)
{
	t_token	*token;

	token = (*tokens)->content;
	*redir = ft_calloc(1, sizeof(**redir));
	if (!*redir)
		return (0);
	*tokens = (*tokens)->next;
	(*redir)->type = token_to_redirect(token->type);
	if (!*tokens || ((t_token *)(*tokens)->content)->type != TOKEN_WORD)
	{
		free(*redir);
		return (0);
	}
	return (1);
}

int	handle_redirect(t_list **tokens, t_command *cmd)
{
	t_redirect	*redir;
	t_list		*file_node;
	t_token		*file_tok;

	if (!init_redirect(tokens, &redir))
		return (0);
	file_node = *tokens;
	file_tok = file_node->content;
	redir->filename = copy_word(file_tok->word);
	if (!redir->filename)
	{
		free(redir);
		return (0);
	}
	ft_lstadd_back(&cmd->redirects, ft_lstnew(redir));
	*tokens = file_node->next;
	free(file_node);
	return (1);
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
