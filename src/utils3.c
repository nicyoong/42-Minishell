/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 18:47:47 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/16 19:00:29 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isxdigit(int c)
{
	if ((c >= '0' && c <= '9')
		|| (c >= 'A' && c <= 'F')
		|| (c >= 'a' && c <= 'f'))
		return (1);
	return (0);
}

int  hex_digit(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	c = ft_toupper(c);
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return (-1);
}

const char *skip_prefix(const char *s)
{
	while (ft_isspace(*s))
		s++;
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;
	return (s);
}

long  hex_to_long(const char *hex)
{
	long	result;
	int		d;

	if (!hex)
		return (0);
	result = 0;
	hex = skip_prefix(hex);
	while (*hex)
	{
		d = hex_digit(*hex);
		if (d < 0)
			break;
		result = (result << 4) | d;
		hex++;
	}
	return (result);
}
