/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:24:23 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/20 20:25:36 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;
t_export	*g_export_list = NULL;

void	remove_export(const char *name)
{
	t_export	**prev;
	t_export	*cur;

	prev = &g_export_list;
	while (*prev)
	{
		cur = *prev;
		if (ft_strcmp(cur->name, name) == 0)
		{
			*prev = cur->next;
			free(cur->name);
			free(cur);
			return ;
		}
		prev = &cur->next;
	}
}

void	print_environment(void)
{
	size_t		count;
	t_export	**arr;

	count = count_exports(g_export_list);
	if (count == 0)
		return ;
	arr = list_to_array(g_export_list, count);
	if (!arr)
		return ;
	sort_exports_insertion(arr, count);
	print_exports(arr, count);
	free(arr);
}

t_export	*find_export(const char *name)
{
	t_export	*cur;

	cur = g_export_list;
	while (cur)
	{
		if (ft_strcmp(cur->name, name) == 0)
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

int	execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int	save_in;
	int	save_out;
	int	save_err;
	int	ret;

	ret = 0;
	if (g_export_list == NULL)
		init_export_list_from_environ();
	save_stdio(&save_in, &save_out, &save_err);
	if (setup_redirections(redirects, ctx) < 0)
	{
		restore_stdio(save_in, save_out, save_err);
		ctx->last_exit_status = 1;
		return (1);
	}
	if (!argv[1])
		print_environment();
	else
		ret = process_export_args(argv);
	restore_stdio(save_in, save_out, save_err);
	ctx->last_exit_status = ret;
	return (ret);
}
