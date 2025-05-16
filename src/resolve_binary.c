/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve_binary.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 23:12:45 by tching            #+#    #+#             */
/*   Updated: 2025/05/16 23:42:25 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*free_full_cmd(char *full_cmd, char **dirs)
{
	ft_split_free(dirs);
	return (full_cmd);
}

static char	*free_resolve_from_path_env(char **dirs)
{
	ft_split_free(dirs);
	errno = ENOENT;
	return (NULL);
}

char	*resolve_from_path_env(char *cmd)
{
	char	*path;
	char	**dirs;
	char	*full;
	char	*full_cmd;
	int		i;

	i = 0;
	path = getenv("PATH");
	if (!path)
		return (NULL);
	dirs = ft_split(path, ':');
	if (!dirs)
		return (NULL);
	while (dirs[i])
	{
		full = ft_strjoin(dirs[i], "/");
		full_cmd = ft_strjoin(full, cmd);
		free(full);
		if (access(full_cmd, X_OK) == 0)
			return (free_full_cmd(full_cmd, dirs));
		free(full_cmd);
		i++;
	}
	return (free_resolve_from_path_env(dirs));
}

char	*resolve_binary(char *cmd)
{
	struct stat	st;

	if (ft_strchr(cmd, '/') != NULL)
	{
		if (access(cmd, F_OK) == -1)
			errno = ENOENT;
		else if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
			errno = EISDIR;
		else if (access(cmd, X_OK) == -1)
			errno = EACCES;
		else
			return (ft_strdup(cmd));
		return (NULL);
	}
	else
		return (resolve_from_path_env(cmd));
}
