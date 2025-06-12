/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 00:06:22 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/13 00:07:10 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

const char	*get_path_value(t_executor_ctx *ctx)
{
	t_export	*e;

	e = find_export(ctx, "PATH");
	if (e && e->assigned && e->value && e->value[0] != '\0')
		return (e->value);
	return (NULL);
}

static char	**get_path_dirs(t_executor_ctx *ctx)
{
	const char	*path_val;

	path_val = get_path_value(ctx);
	if (!path_val)
		return (NULL);
	return (ft_split(path_val, ':'));
}

static char	*build_full_cmd(const char *dir, const char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	return (full);
}

char	*resolve_from_path_env(const char *cmd, t_executor_ctx *ctx)
{
	size_t	i;
	char	**dirs;
	char	*full;

	dirs = get_path_dirs(ctx);
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		full = build_full_cmd(dirs[i], cmd);
		if (full && access(full, X_OK) == 0)
			return (free_full_cmd(full, dirs));
		free(full);
		i++;
	}
	return (free_resolve_from_path_env(dirs));
}

char	*resolve_binary(const char *cmd, t_executor_ctx *ctx)
{
	struct stat	st;

	if (strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == -1)
			errno = ENOENT;
		else if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
			errno = EISDIR;
		else if (access(cmd, X_OK) == -1)
			errno = EACCES;
		else
			return (strdup(cmd));
		return (NULL);
	}
	return (resolve_from_path_env(cmd, ctx));
}
