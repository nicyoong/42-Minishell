/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 23:02:59 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 22:33:25 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_internal_env(t_export *head)
{
	while (head)
	{
		if (head->assigned
			&& ft_strcmp(head->name, "COLUMNS") != 0
			&& ft_strcmp(head->name, "LINES") != 0)
			printf("%s=%s\n", head->name, head->value);
		head = head->next;
	}
}

int	execute_env(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int	save_stdin;
	int	save_stdout;
	int	save_stderr;
	int	ret;
	int	heredoc_fd;

	save_stdin = dup(STDIN_FILENO);
	save_stdout = dup(STDOUT_FILENO);
	save_stderr = dup(STDERR_FILENO);
	ret = 0;
	if (setup_redirections(redirects, ctx, &heredoc_fd) < 0)
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
		print_internal_env(ctx->export_list);
	cleanup_redirections(save_stdin, save_stdout, save_stderr);
	return (ctx->last_exit_status = ret);
}
