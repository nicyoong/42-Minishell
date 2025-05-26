/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils1_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:20:14 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 03:01:18 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

extern char	**environ;

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

void	init_export_list_from_environ(void)
{
	char	**e;
	char	*eq;
	size_t	namelen;
	char	*name;

	e = environ;
	while (*e)
	{
		eq = ft_strchr(*e, '=');
		if (!eq)
		{
			e++;
			continue ;
		}
		namelen = eq - *e;
		name = ft_strndup(*e, namelen);
		if (!name)
		{
			e++;
			continue ;
		}
		add_export(name, true);
		free(name);
		e++;
	}
}
