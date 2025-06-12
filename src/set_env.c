/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tiara <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 20:17:32 by tiara             #+#    #+#             */
/*   Updated: 2025/06/12 21:33:41 by tiara            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*create_env_entry(const char *name, const char *value)
{
	size_t	len[3];
	char	*new_entry;

	len[0] = ft_strlen(name);
	len[1] = ft_strlen(value);
	len[2] = len[0] + len[1] + 2;
	new_entry = malloc((len[2]) * sizeof(char *));
	if (!new_entry)
		return (NULL);
	ft_memcpy(new_entry, name, len[0]);
	new_entry[len[0]] = '=';
	ft_memcpy(new_entry + len[0] + 1, value, len[1]);
	new_entry[len[2] - 1] = '\0';
	return (new_entry);
}

static int	find_env_var(const char *name, size_t name_len,
		int overwrite, char *new_entry)
{
	extern char	**environ;
	int			i;

	i = 0;
	while (environ[i])
	{
		if (ft_strncmp(environ[i], name, name_len) == 0
			&& environ[i][name_len] == '=')
		{
			if (!overwrite)
			{
				free(new_entry);
				return (1);
			}
			free(environ[i]);
			environ[i] = new_entry;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	expand_environment(char *new_entry)
{
	extern char	**environ;
	int			count;
	int			i;
	char		**new_environ;

	i = 0;
	count = 0;
	new_environ = malloc((count + 2) * sizeof(char *));
	while (environ[count])
		count++;
	if (!new_environ)
	{
		free(new_entry);
		return (-1);
	}
	while (i < count)
	{
		new_environ[i] = environ[i];
		i++;
	}
	new_environ[count] = new_entry;
	new_environ[count + 1] = NULL;
	environ = new_environ;
	return (0);
}

int	set_env(const char *name, const char *value, int overwrite)
{
	char	*new_entry;

	if (!name || !*name || strchr(name, '='))
		return (-1);
	new_entry = create_env_entry(name, value);
	if (!new_entry)
		return (-1);
	if (find_env_var(name, strlen(name), overwrite, new_entry))
		return (0);
	return (expand_environment(new_entry));
}
