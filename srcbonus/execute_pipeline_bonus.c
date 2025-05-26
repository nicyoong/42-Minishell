/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 21:48:36 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 03:01:03 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

static void	init_pipe_info(t_pipe_info *pinfo, int prev_fd,
		int *pipe_fd, int is_last)
{
	pinfo->prev_fd = prev_fd;
	pinfo->pipe_fd[0] = pipe_fd[0];
	pinfo->pipe_fd[1] = pipe_fd[1];
	pinfo->is_last = is_last;
}

static int	handle_fork_and_setup(t_command *cmd, t_pipe_info *pinfo,
		t_executor_ctx *ctx, pid_t *last_pid)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		setup_child_process(cmd, pinfo, ctx);
	else if (pid < 0)
	{
		perror("fork");
		ctx->last_exit_status = 1;
		return (-1);
	}
	*last_pid = pid;
	return (0);
}

static int	handle_pipeline_iteration(t_list *node, int *prev_fd,
		pid_t *last_pid, t_executor_ctx *ctx)
{
	int			is_last;
	int			pipe_fd[2];
	t_command	*cmd;
	t_pipe_info	pinfo;

	cmd = node->content;
	is_last = (node->next == NULL);
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	if (!is_last && create_pipe(pipe_fd, ctx) < 0)
		return (-1);
	init_pipe_info(&pinfo, *prev_fd, pipe_fd, is_last);
	if (handle_fork_and_setup(cmd, &pinfo, ctx, last_pid) < 0)
		return (-1);
	close_fds_after_fork(prev_fd, pipe_fd, is_last);
	return (0);
}

void	execute_pipeline_commands(t_pipeline *pipeline, t_executor_ctx *ctx)
{
	int		prev_fd;
	pid_t	last_pid;
	t_list	*node;

	prev_fd = -1;
	last_pid = -1;
	node = pipeline->commands;
	while (node)
	{
		if (handle_pipeline_iteration(node, &prev_fd, &last_pid, ctx) < 0)
			return ;
		node = node->next;
	}
	wait_for_children(last_pid, ctx);
}

void	execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx)
{
	char		**argv;
	t_command	*cmd;

	if (ft_lstsize(pipeline->commands) == 1)
	{
		cmd = pipeline->commands->content;
		argv = convert_arguments(cmd->arguments, ctx);
		if (argv && argv[0] && is_builtin(argv[0]))
		{
			execute_builtin(argv, cmd->redirects, ctx);
			ft_split_free(argv);
			return ;
		}
		ft_split_free(argv);
	}
	execute_pipeline_commands(pipeline, ctx);
}
