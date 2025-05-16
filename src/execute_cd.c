/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:44:19 by tching            #+#    #+#             */
/*   Updated: 2025/05/16 23:06:24 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	restore_std_fds(int save_stdin, int save_stdout, int save_stderr)
{
	dup2(save_stdin, STDIN_FILENO);
	dup2(save_stdout, STDOUT_FILENO);
	dup2(save_stderr, STDERR_FILENO);
	close(save_stdin);
	close(save_stdout);
	close(save_stderr);
}

static int	ret_cd_error(void)
{
	perror("cd");
	return (1);
}

static int	ret_arg_error(void)
{
	write (2, "cd: too many arguments\n", 23);
	return (1);
}

static int	ret_no_arg(void)
{
	char	*home;

	home = getenv("HOME");
	if (!home)
		write (2, "cd: HOME not set\n", 17);
	else
		if (chdir(home) != 0)
			perror("cd");
	return (1);
}

int	handle_cd(char **argv, t_list *redirects, t_executor_ctx *ctx)
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
		restore_std_fds(save_stdin, save_stdout, save_stderr);
		ctx->last_exit_status = 1;
		return (1);
	}
	if (!argv[1])
		ret = ret_no_arg();
	else if (argv[2])
		ret = ret_arg_error();
	else
		if (chdir(argv[1]) != 0)
			ret = ret_cd_error();
	restore_std_fds(save_stdin, save_stdout, save_stderr);
	ctx->last_exit_status = ret;
	return (ret);
}
