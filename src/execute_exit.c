/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:30:57 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 01:21:30 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int execute_exit(char **argv, t_executor_ctx *ctx)
{
	int	exit_code;
	int	arg_count;

	exit_code = ctx->last_exit_status;
	arg_count = 0;
	while (argv[arg_count])
		arg_count++;
	arg_count--;
	if (arg_count > 1)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		exit_code = 1;
	}
	else if (arg_count == 1)
	{
		if (!is_valid_integer(argv[1]))
		{
			ft_putstr_fd("minishell: exit: numeric argument required\n", STDERR_FILENO);
			exit_code = 2;
		}
		else
		{
			exit_code = ft_atoi(argv[1]);
			exit_code = exit_code % 256;
			if (exit_code < 0)
				exit_code += 256;
		}
	}
	exit(exit_code);
}
