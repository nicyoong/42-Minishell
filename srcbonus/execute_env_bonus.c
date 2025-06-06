/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_env_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 23:02:59 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 03:00:53 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

static void	print_filtered_env(void)
{
	extern char	**environ;
	char		**env;

	env = environ;
	while (*env != NULL)
	{
		if (ft_strncmp(*env, "_=", 2) == 0
			|| ft_strncmp(*env, "COLUMNS=", 8) == 0
			|| ft_strncmp(*env, "LINES=", 6) == 0)
		{
			env++;
			continue ;
		}
		printf("%s\n", *env);
		env++;
	}
}

int	execute_env(char **argv, t_list *redirects, t_executor_ctx *ctx)
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
		return (ctx->last_exit_status = ret);
	}
	if (argv[1] != NULL)
	{
		write(2, "env: too many arguments\n", 25);
		ret = 1;
	}
	else
		print_filtered_env();
	cleanup_redirections(save_stdin, save_stdout, save_stderr);
	return (ctx->last_exit_status = ret);
}
