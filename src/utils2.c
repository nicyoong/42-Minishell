/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 17:29:59 by tching            #+#    #+#             */
/*   Updated: 2025/05/15 17:04:00 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_whitespace(char c)
{
	return (c == ' '
		|| c == '\t');
}

int	is_operator_char(char c)
{
	return (c == '|'
		|| c == '<'
		|| c == '>');
}

int	is_valid_var_char(char c)
{
	return (ft_isalnum(c)
		|| c == '_');
}

char *ft_strcat(char *dest, const char *src)
{
    char *d = dest;

    while (*d)
        d++;

    while ((*d++ = *src++))
        ;

    return dest;
}
