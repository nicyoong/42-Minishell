/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 20:11:08 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/12 00:35:33 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc_signals_enable(struct sigaction *old_int,
	struct sigaction *old_quit)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, old_int);
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGQUIT, &sa, old_quit);
}

void	heredoc_signals_restore(struct sigaction *old_int,
	struct sigaction *old_quit)
{
	sigaction(SIGINT, old_int, NULL);
	sigaction(SIGQUIT, old_quit, NULL);
}

bool	fork_failed_cleanup(pid_t pid, char *delim, int fds[2])
{
	if (pid < 0)
	{
		free(delim);
		close(fds[0]);
		close(fds[1]);
		return (true);
	}
	return (false);
}

void	execute_heredoc_child(const char *delim, int fds[2])
{
	close(fds[0]);
	heredoc_signals_enable(NULL, NULL);
	read_until_delimiter(delim, fds[1]);
	close(fds[1]);
	_exit(0);
}

int	collect_heredoc_parent(pid_t pid, int fds[2], t_executor_ctx *ctx)
{
	int	status;

	close(fds[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		close(fds[0]);
		ctx->last_exit_status = 130;
		return (-1);
	}
	return (fds[0]);
}
