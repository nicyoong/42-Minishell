/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils4.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 00:54:11 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/11 01:57:57 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_redirect_end(t_token *current, t_token *next)
{
	if (is_redirect(current->type) && next == NULL)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(token_type_to_str(0), 2);
		ft_putstr_fd("'\n", 2);
		return (0);
	}
	return (1);
}

int	check_redirect_redirect(t_token *current, t_token *next)
{
	if (is_redirect(current->type) && next && is_redirect(next->type))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(token_type_to_str(next->type), 2);
		ft_putstr_fd("'\n", 2);
		return (0);
	}
	return (1);
}

int	check_redirect_pipe(t_token *current, t_token *next)
{
	if (is_redirect(current->type) && next && next->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

int	check_pipe_end(t_token *current, t_token *next)
{
	if (current->type == TOKEN_PIPE && next == NULL)
		return (print_syntax_error_token("newline"));
	return (1);
}

int	check_pipe_follow(t_token *current, t_token *next)
{
	if (current->type == TOKEN_PIPE && next && next->type == TOKEN_PIPE)
		return (print_syntax_error_token("|"));
	return (1);
}
