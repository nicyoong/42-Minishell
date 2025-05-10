/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 18:10:31 by tching            #+#    #+#             */
/*   Updated: 2025/05/10 18:22:34 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include "utils.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

void	ft_split_free(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

int	is_builtin(const char *cmd)
{
	return (cmd && (ft_strcmp(cmd, "cd") == 0
			|| ft_strcmp(cmd, "export") == 0
			|| ft_strcmp(cmd, "unset") == 0
			|| ft_strcmp(cmd, "pwd") == 0
			|| ft_strcmp(cmd, "exit") == 0));
}

int	is_valid_identifier(const char *str)
{
	int	i;

	i = 1;
	if (!str || !*str || !(ft_isalpha(*str) || *str == '_'))
		return (0);
	while (str[i])
	{
		if (!(ft_isalnum(str[i]) || str[i] != '_'))
			return (0);
		i++;
	}
	return (1);
}

int	is_valid_integer(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}
