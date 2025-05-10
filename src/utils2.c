/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 17:29:59 by tching            #+#    #+#             */
/*   Updated: 2025/05/10 17:32:23 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include "utils.h"

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

char *ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
    char *tmp = ft_strjoin(s1, s2);
    char *result = ft_strjoin(tmp, s3);
    free(tmp);
    return result;
}

