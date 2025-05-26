/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_exit_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:30:57 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/27 03:00:58 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

int	count_args(char **argv)
{
	int	count;

	count = 0;
	while (argv[count])
		count++;
	return (count);
}

int	handle_too_many_args(void)
{
	ft_putstr_fd("minishell: exit: too many arguments\n", 2);
	return (1);
}

int	handle_invalid_argument(void)
{
	ft_putstr_fd("minishell: exit: numeric argument required\n", 2);
	return (2);
}

int	parse_exit_code(char *arg)
{
	int	code;

	code = ft_atoi(arg);
	code = code % 256;
	if (code < 0)
		code += 256;
	return (code);
}

int	execute_exit(char **argv, t_executor_ctx *ctx)
{
	int	exit_code;
	int	arg_count;

	exit_code = ctx->last_exit_status;
	arg_count = count_args(argv) - 1;
	if (arg_count > 1)
		exit_code = handle_too_many_args();
	else if (arg_count == 1)
	{
		if (!is_valid_integer(argv[1]))
			exit_code = handle_invalid_argument();
		else
			exit_code = parse_exit_code(argv[1]);
	}
	printf("exit\n");
	exit(exit_code);
}
