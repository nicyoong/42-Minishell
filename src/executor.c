#include "minishell.h"

extern char **environ;

int open_redirection_fd(t_redirect_type type, const char *path, t_word *filename, t_executor_ctx *ctx)
{
	int fd = -1;
	int flags;
	mode_t mode = 0644;

	switch (type)
	{
		case REDIR_IN:
			fd = open(path, O_RDONLY);
			break;
		case REDIR_OUT:
			flags = O_WRONLY | O_CREAT | O_TRUNC;
			fd = open(path, flags, mode);
			break;
		case REDIR_APPEND:
			flags = O_WRONLY | O_CREAT | O_APPEND;
			fd = open(path, flags, mode);
			break;
		case REDIR_HEREDOC:
			fd = process_heredoc(filename, ctx);
			break;
		default:
			fd = -1;
			break;
	}
	return fd;
}

int setup_redirections(t_list *redirects, t_executor_ctx *ctx)
{
	char path[4096];

	for (t_list *node = redirects; node; node = node->next)
	{
		t_redirect *r = node->content;

		if (build_path_from_word(r->filename, path, sizeof(path), ctx) < 0)
		{
			fprintf(stderr, "redirection error: path too long\n");
			return -1;
		}
		char *trimmed = trim_and_validate_path(path);
		if (!trimmed)
		{
			fprintf(stderr, "redirection error: invalid filename\n");
			return -1;
		}
		ft_strncpy(path, trimmed, sizeof(path) - 1);
		path[sizeof(path) - 1] = '\0';
		free(trimmed);

		int fd = open_redirection_fd(r->type, path, r->filename, ctx);
		if (fd < 0)
		{
			perror("redirection error");
			return -1;
		}
		if (duplicate_fd(fd, r->type) < 0)
			return -1;
	}
	return 0;
}

void execute_pipeline_commands(t_pipeline *pipeline, t_executor_ctx *ctx)
{
	int prev_fd = -1;
	pid_t last_pid = -1;
	t_list *node = pipeline->commands;

	while (node)
	{
		t_command *cmd = node->content;
		int is_last = (node->next == NULL);
		int pipe_fd[2];

		if (!is_last && create_pipe(pipe_fd, ctx) < 0)
			return;

		t_pipe_info pinfo = {
			.prev_fd = prev_fd,
			.pipe_fd = {pipe_fd[0], pipe_fd[1]},
			.is_last = is_last
		};

		pid_t pid = fork();
		if (pid == 0)
			setup_child_process(cmd, &pinfo, ctx);
		else if (pid < 0)
		{
			perror("fork");
			ctx->last_exit_status = 1;
			return;
		}

		close_fds_after_fork(&prev_fd, pipe_fd, is_last);
		last_pid = pid;
		node = node->next;
	}
	wait_for_children(last_pid, ctx);
}

void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx)
{
	if (ft_lstsize(pipeline->commands) == 1) {
		t_command *cmd = pipeline->commands->content;
		char **argv = convert_arguments(cmd->arguments, ctx);
		
		if (argv && argv[0] && is_builtin(argv[0])) {
			execute_builtin(argv, cmd->redirects, ctx);
			ft_split_free(argv);
			return;
		}
		ft_split_free(argv);
	}
	execute_pipeline_commands(pipeline, ctx);
}