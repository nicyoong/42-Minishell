/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decode2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:20:11 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:20:32 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_hex_escape(const char *s, int idx)
{
	return (s[idx] == 'x'
		&& ft_isxdigit(s[idx + 1])
		&& ft_isxdigit(s[idx + 2]));
}

char	decode_hex(const char *s, int *idx)
{
	char	hex[3];

	hex[0] = s[*idx + 1];
	hex[1] = s[*idx + 2];
	hex[2] = '\0';
	*idx += 3;
	return ((char)hex_to_long(hex));
}

char	decode_quote(int *idx)
{
	(*idx)++;
	return ('\'');
}

char	decode_default(const char *s, int *idx)
{
	(*idx)++;
	return (s[*idx - 1]);
}

char	decode_escape(const char *s, int *idx)
{
	char	c;

	c = s[*idx];
	if (c == 'n')
		return (decode_newline(idx));
	if (c == 't')
		return (decode_tab(idx));
	if (is_hex_escape(s, *idx))
		return (decode_hex(s, idx));
	if (c == '\\')
		return (decode_backslash(idx));
	if (c == '\'')
		return (decode_quote(idx));
	return (decode_default(s, idx));
}
