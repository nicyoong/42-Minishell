/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_quoted.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:16:16 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:16:44 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	flush_buffer(t_word *word, char *buffer, int *buf_idx)
{
	if (*buf_idx > 0)
	{
		buffer[*buf_idx] = '\0';
		add_segment(word, LITERAL, buffer);
		*buf_idx = 0;
	}
}

void	prepare_for_expansion(t_word *word, char *buffer, int *buf_idx, int *i)
{
	flush_buffer(word, buffer, buf_idx);
	(*i)++;
}

int	process_quoted_content(const char *input, int *i,
	char quote_type, t_word *word)
{
	char	buffer[1024];
	int		buf_idx;

	buf_idx = 0;
	(*i)++;
	while (input[*i] && input[*i] != quote_type)
	{
		if (quote_type == '"' && input[*i] == '$')
		{
			prepare_for_expansion(word, buffer, &buf_idx, i);
			expand_variable(input, i, word);
		}
		else
			buffer[buf_idx++] = input[(*i)++];
	}
	if (buf_idx > 0)
	{
		buffer[buf_idx] = '\0';
		add_segment(word, LITERAL, buffer);
	}
	if (input[*i] != quote_type)
		return (0);
	(*i)++;
	return (1);
}
