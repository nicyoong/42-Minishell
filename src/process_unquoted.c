/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_unquoted.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:21:02 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:23:09 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_ansi_c_quote(const char *input, int *i, t_word *word)
{
	char	buffer[1024];
	size_t	buf_idx;
	char	decoded;

	buf_idx = 0;
	*i += 2;
	while (input[*i] && input[*i] != '\'')
	{
		if (input[*i] == '\\')
		{
			(*i)++;
			decoded = decode_escape(input, i);
			buffer[buf_idx++] = decoded;
		}
		else
			buffer[buf_idx++] = input[(*i)++];
		if (buf_idx >= sizeof(buffer) - 1)
			break ;
	}
	buffer[buf_idx] = '\0';
	add_segment(word, LITERAL, buffer);
	if (input[*i] == '\'')
		(*i)++;
}

void	process_unquoted_segment(const char *input, int *i, t_word *word)
{
	char	buffer[1024];
	int		buf_idx;

	buf_idx = 0;
	while (input[*i]
		&& !is_whitespace(input[*i])
		&& !is_operator_char(input[*i])
		&& input[*i] != '\''
		&& input[*i] != '"')
	{
		if (input[*i] == '$' && input[*i + 1] == '\'')
		{
			flush_buffer(word, buffer, &buf_idx);
			process_ansi_c_quote(input, i, word);
		}
		else if (input[*i] == '$')
		{
			flush_buffer(word, buffer, &buf_idx);
			prepare_for_expansion(word, buffer, &buf_idx, i);
			expand_variable(input, i, word);
		}
		else
			buffer[buf_idx++] = input[(*i)++];
	}
	flush_buffer(word, buffer, &buf_idx);
}
