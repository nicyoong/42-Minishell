/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 23:02:59 by tching            #+#    #+#             */
/*   Updated: 2025/05/15 23:18:54 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_filtered_env(void)
{
	extern char	**environ;
	char		**env;

	env = environ;
	while (*env != NULL)
	{
		if (strncmp(*env, "_=", 2) == 0
			|| strncmp(*env, "COLUMNS=", 8) == 0
			|| strncmp(*env, "LINES=", 6) == 0)
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
		cleanup_redirections(save_stdin, save_stdout, save_stderr, ctx, ret);
		return (ret);
	}
	if (argv[1] != NULL)
	{
		write(2, "env: too many arguments\n", 25);
		ret = 1;
	}
	else
		print_filtered_env();
	cleanup_redirections(save_stdin, save_stdout, save_stderr, ctx, ret);
	return (ret);
}
