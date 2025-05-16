/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 22:35:05 by tching            #+#    #+#             */
/*   Updated: 2025/05/16 23:33:47 by nyoong           ###   ########.fr       */
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
		if (unsetenv(argv[i]) != 0)
		{
			perror("unset");
			ret = 1;
		}
		else
			remove_export(argv[i]);
		i++;
	}
	ctx->last_exit_status = ret;
	return (ret);
}
