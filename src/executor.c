#include "minishell.h"

extern char **environ;

char *resolve_delimiter_word(t_word *delimiter_word, t_executor_ctx *ctx)
{
	char buffer[1024] = {0};
	t_list *seg = delimiter_word->segments;

	while (seg)
	{
		t_segment *s = seg->content;
		char *resolved = NULL;

		if (s->type == VARIABLE)
		{
			resolved = getenv(s->value);
			if (!resolved)
				resolved = "";
		}
		else if (s->type == EXIT_STATUS)
			resolved = ft_itoa(ctx->last_exit_status);
		else
			resolved = s->value;
		ft_strcat(buffer, resolved);
		if (s->type == EXIT_STATUS)
			free(resolved);
		seg = seg->next;
	}
	return strdup(buffer);
}

void read_until_delimiter(const char *delim, int fd_write)
{
	char *line;

	while (1) {
		line = readline("> ");
		if (!line || strcmp(line, delim) == 0)
		{
			if (line)
				free(line);
			break;
		}
		write(fd_write, line, strlen(line));
		write(fd_write, "\n", 1);
		free(line);
	}
}

int process_heredoc(t_word *delimiter_word, t_executor_ctx *ctx)
{
	int fds[2];
	pipe(fds);

	char *delim = resolve_delimiter_word(delimiter_word, ctx);
	read_until_delimiter(delim, fds[1]);
	free(delim);

	close(fds[1]);
	return fds[0];
}

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
	int target = (type == REDIR_IN || type == REDIR_HEREDOC) ? STDIN_FILENO : STDOUT_FILENO;

	if (dup2(fd, target) < 0)
	{
		perror("dup2 error");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
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

char    *get_segment_value(t_segment *s, t_executor_ctx *ctx)
{
        char    *val;

        if (s->type == VARIABLE)
        {
                val = getenv(s->value);
                if (val)
                        return (val);
                else
                        return ("");
        }
        else if (s->type == EXIT_STATUS)
                return (ft_itoa(ctx->last_exit_status));
        else
                return (s->value);
}

char *concatenate_segments(t_word *word, t_executor_ctx *ctx)
{
	char buffer[1024] = {0};

	t_list *seg = word->segments;
	while (seg)
	{
		t_segment *s = seg->content;
		char *val = get_segment_value(s, ctx);

		ft_strcat(buffer, val);
		if (s->type == EXIT_STATUS)
			free(val);
		seg = seg->next;
	}

	return ft_strdup(buffer);
}

char **convert_arguments(t_list *args, t_executor_ctx *ctx)
{
	int size = ft_lstsize(args);
	char **argv = ft_calloc(size + 1, sizeof(char *));
	int i = 0;

	t_list *node = args;
	while (node)
	{
		t_word *word = node->content;
		char *arg = concatenate_segments(word, ctx);

		if (arg[0] != '\0')
			argv[i++] = arg;
		else
			free(arg);
		node = node->next;
	}
	return argv;
}

int execute_builtin(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	if (ft_strcmp(argv[0], "cd") == 0) {
		return handle_cd(argv, redirects, ctx);
	} else if (ft_strcmp(argv[0], "export") == 0) {
		return execute_export(argv, redirects, ctx);
	} else if (ft_strcmp(argv[0], "unset") == 0) {
		return execute_unset(argv, redirects, ctx);
	} else if (ft_strcmp(argv[0], "pwd") == 0) {
		return execute_pwd(argv, redirects, ctx);
	} else if (ft_strcmp(argv[0], "exit") == 0) {
		execute_exit(argv, ctx);
		return 0;
	} else if (ft_strcmp(argv[0], "echo") == 0) {
		return execute_echo(argv, redirects, ctx);
	} else if (ft_strcmp(argv[0], "env") == 0) {
		return execute_env(argv, redirects, ctx);
	}
	return -1;
}

void handle_invalid_arguments(char **argv)
{
	if (!argv || !argv[0] || argv[0][0] == '\0') {
		ft_split_free(argv);
		exit(0);
	}
}

void handle_builtin_command(char **argv, t_command *cmd, t_executor_ctx *ctx)
{
	if (is_builtin(argv[0])) {
		int status = execute_builtin(argv, cmd->redirects, ctx);
		ft_split_free(argv);
		exit(status);
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

