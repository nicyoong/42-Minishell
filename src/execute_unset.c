/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 22:35:05 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 20:25:25 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_unset(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int	ret;
	int	i;

	(void)redirects;
	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (find_export(ctx, argv[i]))
			remove_export(ctx, argv[i]);
		/*else
		{
			ft_putstr_fd("unset: no such variable: ", STDERR_FILENO);
			ft_putendl_fd(argv[i], STDERR_FILENO);
			ret = 1;
		}*/
		i++;
	}
	ctx->last_exit_status = ret;
	return (ret);
}
