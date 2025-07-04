/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:24:23 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/12 01:00:59 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	remove_export(t_executor_ctx *ctx, const char *name)
{
	t_export	**prev;
	t_export	*cur;

	prev = &ctx->export_list;
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

void	print_environment(t_executor_ctx *ctx)
{
	size_t		count;
	t_export	**arr;

	count = count_exports(ctx->export_list);
	if (count == 0)
		return ;
	arr = list_to_array(ctx->export_list, count);
	if (!arr)
		return ;
	sort_exports_insertion(arr, count);
	print_exports(arr, count);
	free(arr);
}

t_export	*find_export(t_executor_ctx *ctx, const char *name)
{
	t_export	*cur;

	cur = ctx->export_list;
	while (cur)
	{
		if (ft_strcmp(cur->name, name) == 0)
			return (cur);
		cur = cur->next;
	}
	return (NULL);
}

void	add_export(t_executor_ctx *ctx, const char *name, bool assigned)
{
	t_export	*ent;

	ent = find_export(ctx, name);
	if (ent)
		ent->assigned = ent->assigned || assigned;
	else
	{
		ent = malloc(sizeof(*ent));
		if (!ent)
			return ;
		ent->name = ft_strdup(name);
		if (!ent->name)
		{
			free(ent);
			return ;
		}
		ent->assigned = assigned;
		ent->next = ctx->export_list;
		ctx->export_list = ent;
	}
}

int	execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int				save[3];
	int				ret;
	int				heredoc_fd;
	t_redir_status	st;

	ret = 0;
	if (ctx->export_list == NULL)
		init_export_list_from_environ(ctx);
	save_stdio(&save[0], &save[1], &save[2]);
	st = setup_redirections(redirects, ctx, &heredoc_fd);
	if (st == HEREDOC_ABORT)
		return (handle_heredoc_abort(save, ctx));
	if (st == REDIR_ERROR)
	{
		ctx->last_exit_status = 1;
		restore_stdio(save[0], save[1], save[2]);
		return (1);
	}
	if (!argv[1])
		print_environment(ctx);
	else
		ret = process_export_args(argv, ctx);
	restore_stdio(save[0], save[1], save[2]);
	ctx->last_exit_status = ret;
	return (ret);
}
