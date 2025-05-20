/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:20:14 by tching            #+#    #+#             */
/*   Updated: 2025/05/20 13:20:30 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;
t_export	*g_export_list = NULL;

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

t_export	*find_export(const char *name)
{
	t_export	*cur;

	cur = g_export_list;
	while (cur)
	{
		if (strcmp(cur->name, name) == 0)
			return (cur);
		cur = cur->next;
	}
	return (NULL);
}

void	add_export(const char *name, bool assigned)
{
	t_export	*ent;

	ent = find_export(name);
	if (ent)
		ent->assigned = ent->assigned || assigned;
	else
	{
		ent = malloc(sizeof(*ent));
		if (!ent)
			return ;
		ent->name = ft_strdup(name);
		ent->assigned = assigned;
		ent->next = g_export_list;
		g_export_list = ent;
	}
}

void	init_export_list_from_environ(void)
{
	char	**e;
	char	*eq;
	size_t	namelen;

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
		char *name = ft_strndup(*e, namelen);
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