/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 17:29:59 by tching            #+#    #+#             */
/*   Updated: 2025/05/16 00:17:50 by nyoong           ###   ########.fr       */
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

char *ft_strncpy(char *dest, const char *src, size_t n)
{
    size_t i = 0;

    while (i < n && src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
	}
    while (i < n)
    {
        dest[i] = '\0';
        i++;
    }
    return dest;
}
