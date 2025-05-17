#include "minishell.h"

extern char **environ;

int build_path_from_word(t_word *word, char *buffer, size_t bufsize, t_executor_ctx *ctx)
{
	buffer[0] = '\0';
	for (t_list *seg = word->segments; seg; seg = seg->next)
	{
		t_segment *s = seg->content;
		char *resolved = NULL;
		if (s->type == VARIABLE)
		{
			resolved = getenv(s->value);
			if (!resolved) resolved = "";
		}
		else if (s->type == EXIT_STATUS)
			resolved = ft_itoa(ctx->last_exit_status);
		else
			resolved = s->value;
		if (strlen(buffer) + strlen(resolved) >= bufsize)
		{
			if (s->type == EXIT_STATUS) free(resolved);
			return -1;
		}
		ft_strcat(buffer, resolved);
		if (s->type == EXIT_STATUS)
			free(resolved);
	}
	return 0;
}

char *trim_and_validate_path(const char *path)
{
	char *trimmed = ft_strtrim(path, " \t\n\r");
	if (!trimmed || trimmed[0] == '\0')
	{
		if (trimmed) free(trimmed);
		return NULL;
	}
	return trimmed;
}

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

int duplicate_fd(int fd, t_redirect_type type)
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
		return -1;
	}
	close(fd);
	return (0);
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

void cleanup_redirections(int save_stdin, int save_stdout, int save_stderr, t_executor_ctx *ctx, int ret)
{
	dup2(save_stdin, STDIN_FILENO);
	dup2(save_stdout, STDOUT_FILENO);
	dup2(save_stderr, STDERR_FILENO);
	close(save_stdin);
	close(save_stdout);
	close(save_stderr);
	ctx->last_exit_status = ret;
}

void handle_invalid_arguments(char **argv)
{
	if (!argv || !argv[0] || argv[0][0] == '\0') {
		ft_split_free(argv);
		exit(0);
	}
}

void handle_path_errors(char *path, char **argv)
{
	if (!path) {
		if (errno == EACCES) {
			fprintf(stderr, "minishell: %s: Permission denied\n", argv[0]);
			ft_split_free(argv);
			exit(126);
		}
		else if (errno == EISDIR) {
			fprintf(stderr, "minishell: %s: Is a directory\n", argv[0]);
			ft_split_free(argv);
			exit(126);
		}
		else if (errno == ENOENT) {
			if (strchr(argv[0], '/') != NULL)
				fprintf(stderr, "minishell: %s: No such file or directory\n", argv[0]);
			else
				fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
			ft_split_free(argv);
			exit(127);
		}
		else {
			fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
			ft_split_free(argv);
			exit(127);
		}
	}
}

void execute_binary(char *path, char **argv)
{
	execve(path, argv, environ);
	perror("execve");
	exit(126);
}

void execute_child(t_command *cmd, t_executor_ctx *ctx)
{
	char **argv = convert_arguments(cmd->arguments, ctx);

	handle_invalid_arguments(argv);
	handle_builtin_command(argv, cmd, ctx);

	char *path = resolve_binary(argv[0]);
	handle_path_errors(path, argv);

	execute_binary(path, argv);
}

int create_pipe(int pipe_fd[2], t_executor_ctx *ctx)
{
	if (pipe(pipe_fd) < 0)
	{
		perror("pipe");
		ctx->last_exit_status = 1;
		return -1;
	}
	return 0;
}

void setup_child_process(t_command *cmd, int prev_fd, int pipe_fd[2], int is_last, t_executor_ctx *ctx)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (!is_last)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
	if (setup_redirections(cmd->redirects, ctx) < 0)
		exit(1);
	execute_child(cmd, ctx);
}

void close_fds_after_fork(int *prev_fd, int pipe_fd[2], int is_last)
{
	if (*prev_fd != -1) close(*prev_fd);
	if (!is_last) {
		close(pipe_fd[1]);
		*prev_fd = pipe_fd[0];
	} else {
		*prev_fd = -1;
	}
}

void wait_for_children(pid_t last_pid, t_executor_ctx *ctx)
{
	int status;
	waitpid(last_pid, &status, 0);
	if (WIFEXITED(status))
		ctx->last_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		ctx->last_exit_status = 128 + sig;
	}
	else
		ctx->last_exit_status = 1;
	while (wait(NULL) > 0)
		;
}

void execute_pipeline_commands(t_pipeline *pipeline, t_executor_ctx *ctx)
{
	int prev_fd = -1;
	pid_t last_pid = -1;

	t_list *node = pipeline->commands;
	while (node) {
		t_command *cmd = node->content;
		int is_last = (node->next == NULL);
		int pipe_fd[2];

		if (!is_last && create_pipe(pipe_fd, ctx) < 0)
			return;
		pid_t pid = fork();
		if (pid == 0)
			setup_child_process(cmd, prev_fd, pipe_fd, is_last, ctx);
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
    if (ft_lstsize(pipeline->commands) == 1)
	{
        t_command *cmd = pipeline->commands->content;
        char **argv = convert_arguments(cmd->arguments, ctx);
		
        if (argv && argv[0] && is_builtin(argv[0]))
		{
            int save_in  = dup(STDIN_FILENO);
            int save_out = dup(STDOUT_FILENO);
            int save_err = dup(STDERR_FILENO);

            if (setup_redirections(cmd->redirects, ctx) < 0)
			{
                cleanup_redirections(save_in, save_out, save_err, ctx, 1);
                ft_split_free(argv);
                return;
            }
            int status = execute_builtin(argv, cmd->redirects, ctx);
            cleanup_redirections(save_in, save_out, save_err, ctx, status);
            ft_split_free(argv);
            return;
        }
        ft_split_free(argv);
    }
    execute_pipeline_commands(pipeline, ctx);
}

// Old execute pipeline in case any bug change to this
// void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx)
// {
// 	if (ft_lstsize(pipeline->commands) == 1) {
// 		t_command *cmd = pipeline->commands->content;
// 		char **argv = convert_arguments(cmd->arguments, ctx);
		
// 		if (argv && argv[0] && is_builtin(argv[0])) {
// 			execute_builtin(argv, cmd->redirects, ctx);
// 			ft_split_free(argv);
// 			return;
// 		}
// 		ft_split_free(argv);
// 	}
// 	execute_pipeline_commands(pipeline, ctx);
// }

