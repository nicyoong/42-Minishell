/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shlvl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 01:35:14 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/12 22:08:56 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_current_shlvl(t_executor_ctx *ctx)
{
	t_export	*e;

	e = find_export(ctx, "SHLVL");
	if (!e || !e->assigned || !e->value)
		return (0);
	return (ft_atoi(e->value));
}

void	set_shlvl(t_executor_ctx *ctx, int new_level)
{
	char	*buf;

	buf = ft_itoa(new_level);
	add_export(ctx, "SHLVL", buf, true);
	free(buf);
}

void	init_shell_level(t_executor_ctx *ctx)
{
	int	lvl;

	lvl = get_current_shlvl(ctx);
	set_shlvl(ctx, lvl + 1);
}
