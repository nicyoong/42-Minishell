/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:20:14 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 20:38:51 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	save_stdio(int *in, int *out, int *err)
{
	*in = dup(STDIN_FILENO);
	*out = dup(STDOUT_FILENO);
	*err = dup(STDERR_FILENO);
}

void	restore_stdio(int in, int out, int err)
{
	dup2(in, STDIN_FILENO);
	dup2(out, STDOUT_FILENO);
	dup2(err, STDERR_FILENO);
	close(in);
	close(out);
	close(err);
}

void	handle_env_entry(t_executor_ctx *ctx, const char *env_entry)
{
	char	*name;
	char	*value;
	char	*eq;
	size_t	name_len;

	eq = ft_strchr(env_entry, '=');
	if (eq)
	{
		name_len = eq - env_entry;
		name = ft_strndup(env_entry, name_len);
		value = eq + 1;
		if (name)
		{
			add_export(ctx, name, value, true);
			free(name);
		}
	}
	else
		add_export(ctx, env_entry, NULL, false);
}

void	init_export_list_from_environ(t_executor_ctx *ctx)
{
	extern char	**environ;
	size_t		i;

	i = 0;
	while (environ[i])
	{
		handle_env_entry(ctx, environ[i]);
		i++;
	}
}

// void	init_export_list_from_environ(t_executor_ctx *ctx)
// {
// 	extern char	**environ;
// 	size_t		i;
// 	size_t		name_len;
// 	char		*name;
// 	char		*eq;

// 	i = 0;
// 	while (environ[i])
// 	{
// 		eq = ft_strchr(environ[i], '=');
// 		if (eq)
// 		{
// 			name_len = eq - environ[i];
// 			name = ft_strndup(environ[i], name_len);
// 			if (name)
// 			{
// 				add_export(ctx, name, true);
// 				free(name);
// 			}
// 		}
// 		else
// 			add_export(ctx, environ[i], false);
// 		i++;
// 	}
// }
