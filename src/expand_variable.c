/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:15:00 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:15:35 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse_exit_status(const char *input, int *i, t_word *word)
{
	(void) input;
	add_segment(word, EXIT_STATUS, "$?");
	(*i)++;
}

void	parse_variable_name(const char *input, int *i, t_word *word)
{
	char	var[1024];
	int		var_idx;

	var_idx = 0;
	while (is_valid_var_char(input[*i]))
		var[var_idx++] = input[(*i)++];
	var[var_idx] = '\0';
	add_segment(word, VARIABLE, var);
}

void	expand_variable(const char *input, int *i, t_word *word)
{
	if (input[*i] == '?')
		parse_exit_status(input, i, word);
	else if (is_valid_var_char(input[*i]))
		parse_variable_name(input, i, word);
	else
		add_segment(word, LITERAL, "$");
}
