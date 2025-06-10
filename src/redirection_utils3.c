/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils3.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 23:34:17 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/11 02:01:01 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*token_type_to_str(t_token_type type)
{
	if (type == TOKEN_PIPE)
		return ("|");
	if (type == TOKEN_REDIRECT_IN)
		return ("<");
	if (type == TOKEN_REDIRECT_OUT)
		return (">");
	if (type == TOKEN_REDIRECT_APPEND)
		return (">>");
	if (type == TOKEN_REDIRECT_HEREDOC)
		return ("<<");
	if (type == TOKEN_HERESTRING)
		return ("<<<");
	return ("newline");
}

int	print_syntax_error_token(char *tok)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(tok, 2);
	ft_putstr_fd("'\n", 2);
	return (0);
}

int	check_token_pair(t_token *current, t_token *next, int first)
{
	if (first && current->type == TOKEN_PIPE)
		return (print_syntax_error_token("|"));
	if (!check_redirect_end(current, next))
		return (0);
	if (!check_redirect_redirect(current, next))
		return (0);
	if (!check_redirect_pipe(current, next))
		return (0);
	if (!check_pipe_follow(current, next))
		return (0);
	if (!check_pipe_end(current, next))
		return (0);
	return (1);
}

int	validate_syntax(t_list *tokens)
{
	t_list	*node;
	t_token	*current;
	t_token	*next;
	int		first;

	node = tokens;
	first = 1;
	while (node)
	{
		current = node->content;
		if (node->next)
			next = node->next->content;
		else
			next = NULL;
		if (!check_token_pair(current, next, first))
			return (0);
		first = 0;
		node = node->next;
	}
	return (1);
}

int	handle_syntax_error(t_list *tokens, char *full_line, t_executor_ctx *ctx)
{
	if (!validate_syntax(tokens))
	{
		ft_lstclear(&tokens, free_token);
		free(full_line);
		ctx->last_exit_status = 2;
		return (0);
	}
	return (1);
}
