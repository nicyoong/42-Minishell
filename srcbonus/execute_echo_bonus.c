/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_echo_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 22:48:48 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 03:00:51 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

static int	run_echo_command(char **argv)
{
	int	newline;
	int	i;

	newline = 1;
	i = 1;
	while (argv[i] && ft_strcmp(argv[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (argv[i])
	{
		printf("%s", argv[i]);
		if (argv[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

int	execute_echo(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int	save_stdin;
	int	save_stdout;
	int	save_stderr;
	int	ret;

	save_stdin = dup(STDIN_FILENO);
	save_stdout = dup(STDOUT_FILENO);
	save_stderr = dup(STDERR_FILENO);
	ret = 0;
	if (setup_redirections(redirects, ctx) < 0)
	{
		ret = 1;
		cleanup_redirections(save_stdin, save_stdout, save_stderr);
		ctx->last_exit_status = ret;
		return (ret);
	}
	ret = run_echo_command(argv);
	cleanup_redirections(save_stdin, save_stdout, save_stderr);
	ctx->last_exit_status = ret;
	return (ret);
}
