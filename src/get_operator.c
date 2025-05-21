/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_operator.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:23:22 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:23:54 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token_type	parse_redirect_in(const char *input, int *i)
{
	if (input[*i + 1] == '<')
	{
		(*i) += 2;
		return (TOKEN_REDIRECT_HEREDOC);
	}
	(*i)++;
	return (TOKEN_REDIRECT_IN);
}

t_token_type	parse_redirect_out(const char *input, int *i)
{
	if (input[*i + 1] == '>')
	{
		(*i) += 2;
		return (TOKEN_REDIRECT_APPEND);
	}
	(*i)++;
	return (TOKEN_REDIRECT_OUT);
}

t_token_type	get_operator(const char *input, int *i)
{
	if (input[*i] == '|')
	{
		(*i)++;
		return (TOKEN_PIPE);
	}
	else if (input[*i] == '<')
		return (parse_redirect_in(input, i));
	else if (input[*i] == '>')
		return (parse_redirect_out(input, i));
	return (TOKEN_WORD);
}
