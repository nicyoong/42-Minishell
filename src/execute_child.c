/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:57:46 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 00:21:38 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execute_binary(char *path, char **argv)
{
	extern char	**environ;

	execve(path, argv, environ);
	perror("execve");
	exit(126);
}

void	execute_child(t_command *cmd, t_executor_ctx *ctx)
{
	char	**argv;
	char	*path;

	argv = convert_arguments(cmd->arguments, ctx);
	handle_invalid_arguments(argv);
	handle_builtin_command(argv, cmd, ctx);
	path = resolve_binary(argv[0]);
	handle_path_errors(path, argv);
	execute_binary(path, argv);
}

void	setup_child_process(t_command *cmd, t_pipe_info *pinfo,
				t_executor_ctx *ctx)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT,  &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	if (pinfo->prev_fd != -1)
	{
		dup2(pinfo->prev_fd, STDIN_FILENO);
		close(pinfo->prev_fd);
	}
	if (!pinfo->is_last)
	{
		close(pinfo->pipe_fd[0]);
		dup2(pinfo->pipe_fd[1], STDOUT_FILENO);
		close(pinfo->pipe_fd[1]);
	}
	if (setup_redirections(cmd->redirects, ctx) < 0)
		exit(1);
	execute_child(cmd, ctx);
}

void	wait_for_children(pid_t last_pid, t_executor_ctx *ctx)
{
	int	status;
	int	sig;

	waitpid(last_pid, &status, 0);
	if (WIFEXITED(status))
		ctx->last_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(2, "\n", 1);
		else if (sig == SIGQUIT)
			write(2, "Quit (core dumped)\n", 19);
		ctx->last_exit_status = 128 + sig;
	}
	else
		ctx->last_exit_status = 1;
	while (wait(NULL) > 0)
		;
}
