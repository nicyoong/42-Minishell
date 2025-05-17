/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils4.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 21:06:01 by tching            #+#    #+#             */
/*   Updated: 2025/05/17 21:16:09 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	create_pipe(int pipe_fd[2], t_executor_ctx *ctx)
{
	if (pipe(pipe_fd) < 0)
	{
		perror("pipe");
		ctx->last_exit_status = 1;
		return (-1);
	}
	return (0);
}

void	close_fds_after_fork(int *prev_fd, int pipe_fd[2], int is_last)
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (!is_last)
	{
		close(pipe_fd[1]);
		*prev_fd = pipe_fd[0];
	}
	else
		*prev_fd = -1;
}

int	duplicate_fd(int fd, t_redirect_type type)
{
	int	target;

	if (type == REDIR_IN || type == REDIR_HEREDOC)
		target = STDIN_FILENO;
	else
		target = STDOUT_FILENO;
	if (dup2(fd, target) < 0)
	{
		perror("dup2 error");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}
