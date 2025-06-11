/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 22:48:48 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 00:49:45 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	int				save[3];
	int				heredoc_fd;
	t_redir_status	st;

	save[0] = dup(STDIN_FILENO);
	save[1] = dup(STDOUT_FILENO);
	save[2] = dup(STDERR_FILENO);
	st = setup_redirections(redirects, ctx, &heredoc_fd);
	if (st == HEREDOC_ABORT)
		return (handle_heredoc_abort(save, ctx));
	if (st == REDIR_ERROR)
	{
		ctx->last_exit_status = 1;
		cleanup_redirections(save[0], save[1], save[2]);
		return (ctx->last_exit_status);
	}
	ctx->last_exit_status = run_echo_command(argv);
	cleanup_redirections(save[0], save[1], save[2]);
	return (ctx->last_exit_status);
}
