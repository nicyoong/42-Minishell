/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 00:26:39 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/20 00:26:40 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_list	*clear_on_error(t_list **current, t_list **cmds)
{
	if (*current)
		ft_lstclear(current, NULL);
	if (*cmds)
		ft_lstclear(cmds, NULL);
	return (NULL);
}

int	finalize_command(t_list **cmds, t_list **current)
{
	if (!*current)
	{
		write(2, "minishell: syntax error near unexpected token `|'\n", 49);
		return (-1);
	}
	ft_lstadd_back(cmds, ft_lstnew(*current));
	*current = NULL;
	return (0);
}

t_list	*split_commands(t_list *tokens)
{
	t_list	*cmds;
	t_list	*current;
	t_token	*token;

	cmds = NULL;
	current = NULL;
	while (tokens)
	{
		token = tokens->content;
		if (token->type == TOKEN_PIPE)
		{
			if (finalize_command(&cmds, &current) == -1)
				return (clear_on_error(&current, &cmds));
		}
		else
			add_token_to_current(&current, token);
		tokens = tokens->next;
	}
	if (current)
		ft_lstadd_back(&cmds, ft_lstnew(current));
	return (cmds);
}

int	parse_command_group(t_list *curr_group, t_pipeline *pipeline)
{
	t_list		**cmd_tokens;
	t_command	*cmd;

	cmd_tokens = (t_list **)&curr_group->content;
	cmd = parse_command(cmd_tokens);
	if (!cmd || *cmd_tokens != NULL)
		return (1);
	ft_lstadd_back(&pipeline->commands, ft_lstnew(cmd));
	return (0);
}

t_pipeline	*parse(t_list *tokens)
{
	t_pipeline	*pipeline;
	t_list		*cmd_groups;
	t_list		*curr_group;

	pipeline = ft_calloc(1, sizeof(t_pipeline));
	cmd_groups = split_commands(tokens);
	if (!cmd_groups)
	{
		free(pipeline);
		return (NULL);
	}
	curr_group = cmd_groups;
	while (curr_group)
	{
		if (parse_command_group(curr_group, pipeline))
		{
			ft_lstclear(&pipeline->commands, free_command);
			free(pipeline);
			ft_lstclear(&cmd_groups, clear_token_list);
			return (NULL);
		}
		curr_group = curr_group->next;
	}
	ft_lstclear(&cmd_groups, clear_token_list);
	return (pipeline);
}

// const char *redirect_type_str(t_redirect_type type)
// {
// 	const char *names[] = {
// 		[REDIR_IN] = "INPUT",
// 		[REDIR_OUT] = "OUTPUT",
// 		[REDIR_APPEND] = "APPEND",
// 		[REDIR_HEREDOC] = "HEREDOC"
// 	};
// 	return names[type];
// }
