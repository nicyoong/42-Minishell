/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 00:18:02 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/11 02:43:35 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	process_token(t_list **tokens, t_command *cmd)
{
	t_list	*head;
	t_token	*token;

	head = *tokens;
	token = head->content;
	if (is_redirect(token->type))
	{
		if (!process_redirect(tokens, cmd, head))
			return (0);
	}
	else if (token->type == TOKEN_WORD)
	{
		if (!process_word(tokens, cmd, head))
			return (0);
	}
	else
	{
		free(head);
		free_command(cmd);
		return (0);
	}
	return (1);
}

t_command	*parse_command(t_list **tokens)
{
	t_command	*cmd;

	cmd = ft_calloc(1, sizeof(t_command));
	if (!cmd)
		return (NULL);
	while (*tokens)
	{
		if (!process_token(tokens, cmd))
			return (NULL);
	}
	return (cmd);
}

void	clear_token_list(void *content)
{
	t_list	*lst;

	lst = content;
	ft_lstclear(&lst, free_token);
}

void	add_token_to_current(t_list **current, t_token *token)
{
	t_list	*new_node;

	new_node = ft_lstnew(token);
	if (!*current)
		*current = new_node;
	else
		ft_lstadd_back(current, new_node);
}

int	finalize_current_command(t_list **cmds, t_list **current)
{
	if (!*current)
		return (-1);
	ft_lstadd_back(cmds, ft_lstnew(*current));
	*current = NULL;
	return (0);
}
