#include "minishell.h"

extern char **environ;

int process_heredoc(t_word *delimiter_word, t_executor_ctx *ctx)
{
    int fds[2];
    pipe(fds);
    
    char buffer[1024] = {0};
    t_list *seg = delimiter_word->segments;
    while (seg)
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

        ft_strcat(buffer, resolved);

        if (s->type == EXIT_STATUS)
            free(resolved);

        seg = seg->next;
    }
    char *delim = buffer;
    char *line;
    while (1) {
        line = readline("> ");
        if (!line || strcmp(line, delim) == 0)
        {
            if (line)
                free (line);
            break;
        }
        write(fds[1], line, strlen(line));
        write(fds[1], "\n", 1);
        free(line);
    }
    close(fds[1]);
    return fds[0];
}

char	*resolve_segment(t_segment *seg, t_executor_ctx *ctx)
{
	char	*value;

	if (seg->type == VARIABLE)
	{
		value = getenv(seg->value);
		if (value)
			return (ft_strdup(value));
		else
			return (ft_strdup(""));
	}
	else if (seg->type == EXIT_STATUS)
		return (ft_itoa(ctx->last_exit_status));
	return (ft_strdup(seg->value));
}

int setup_redirections(t_list *redirects, t_executor_ctx *ctx)
{
    for (t_list *node = redirects; node; node = node->next) 
    {
        t_redirect *r = node->content;
        char path[1024] = {0};

        t_list *seg = r->filename->segments;
        while (seg)
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

            ft_strcat(path, resolved);

            if (s->type == EXIT_STATUS)
                free(resolved);

            seg = seg->next;
        }
        char *trimmed = ft_strtrim(path, " \t\n\r");
        if (!trimmed)
        {
            fprintf(stderr, "redirection error: invalid filename\n");
            return -1;
        }
        strncpy(path, trimmed, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
        free(trimmed);

        int fd = -1;
        int flags = 0;
        mode_t mode = 0644;

        if (r->type == REDIR_IN) {
            fd = open(path, O_RDONLY);
        } 
        else if (r->type == REDIR_OUT) {
            flags = O_WRONLY | O_CREAT | O_TRUNC;
            fd = open(path, flags, mode);
        } 
        else if (r->type == REDIR_APPEND) {
            flags = O_WRONLY | O_CREAT | O_APPEND;
            fd = open(path, flags, mode);
        } 
        else if (r->type == REDIR_HEREDOC) {
            fd = process_heredoc(r->filename, ctx);
        } 
        else {
            fprintf(stderr, "Unknown redirection type\n");
            return -1;
        }

        if (fd < 0)
        {
            perror("redirection error");
            return -1;
        }

        int target = (r->type == REDIR_IN || r->type == REDIR_HEREDOC) 
                   ? STDIN_FILENO : STDOUT_FILENO;
        
        if (dup2(fd, target) < 0) {
            perror("dup2 error");
            close(fd);
            return -1;
        }
        close(fd);
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

int execute_echo(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);
    int ret = 0;
    int newline = 1;
    int i = 1;

    if (setup_redirections(redirects, ctx) < 0)
    {
        ret = 1;
        cleanup_redirections(save_stdin, save_stdout, save_stderr, ctx, ret);
        return ret;
    }
    if (argv[i] && strcmp(argv[i], "-n") == 0)
    {
        newline = 0;
        i++;
    }
    while (argv[i])
    {
        printf("%s", argv[i]);
        if (argv[i + 1]) printf(" ");
        i++;
    }
    if (newline)
        printf("\n");
    cleanup_redirections(save_stdin, save_stdout, save_stderr, ctx, ret);
    return ret;
}

int execute_env(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);
    int ret = 0;

    if (setup_redirections(redirects, ctx) < 0)
    {
        ret = 1;
        cleanup_redirections(save_stdin, save_stdout, save_stderr, ctx, ret);
        return ret;
    }
    if (argv[1] != NULL)
    {
        fprintf(stderr, "env: too many arguments\n");
        ret = 1;
        cleanup_redirections(save_stdin, save_stdout, save_stderr, ctx, ret);
        return ret;
    }
    char **env = environ;
    while (*env != NULL) {
        if (ft_strncmp(*env, "_=", 2) == 0 ||
            ft_strncmp(*env, "COLUMNS=", 8) == 0 ||
            ft_strncmp(*env, "LINES=", 6) == 0) {
            env++;
            continue;
        }
        printf("%s\n", *env);
        env++;
    }
    cleanup_redirections(save_stdin, save_stdout, save_stderr, ctx, ret);
    return ret;
}

int execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);
    int ret = 0;

    if (setup_redirections(redirects, ctx) < 0)
    {
        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        dup2(save_stderr, STDERR_FILENO);
        close(save_stdin);
        close(save_stdout);
        close(save_stderr);
        ctx->last_exit_status = 1;
        return 1;
    }
    if (!argv[1])
    {
        extern char **environ;
        char **env = environ;
        while (*env) {
            printf("%s\n", *env);
            env++;
        }
    }
    else
    {
        int i = 1;
        while (argv[i]) {
            char *arg = argv[i];
            char *eq = strchr(arg, '=');
            char *name = NULL;
            char *error_part = NULL;
            int name_invalid = 0;

            if (eq) {
                name = ft_substr(arg, 0, eq - arg);
                error_part = ft_substr(arg, 0, eq - arg + 1);
            } else {
                name = ft_strdup(arg);
                error_part = ft_strdup(arg);
            }

            if (!is_valid_identifier(name)) {
                fprintf(stderr, "export: '%s': not a valid identifier\n", error_part);
                ret = 1;
                name_invalid = 1;
            }
            free(error_part);

            if (name_invalid) {
                free(name);
                i++;
                continue;
            }

            if (eq) {
                char *value = eq + 1;
                if (setenv(name, value, 1) != 0) {
                    perror("export");
                    ret = 1;
                }
            } else {
                char *current = getenv(name);
                if (setenv(name, current ? current : "", 1) != 0) {
                    perror("export");
                    ret = 1;
                }
            }

            free(name);
            i++;
        }
    }
    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);
    dup2(save_stderr, STDERR_FILENO);
    close(save_stdin);
    close(save_stdout);
    close(save_stderr);

    ctx->last_exit_status = ret;
    return ret;
}

int execute_unset(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int ret;
    int i;

    (void)redirects;
    ret = 0;
    i = 1;
    while (argv[i])
    {
        if (unsetenv(argv[i]) != 0)
        {
            perror("unset");
            ret = 1;
        }
        i++;
    }
    ctx->last_exit_status = ret;
    return (ret);
}

char *resolve_from_path_env(char *cmd)
{
    char *path;
    
    path = getenv("PATH");
    if (!path)
        return NULL;
    char **dirs = ft_split(path, ':');
    if (!dirs)
        return NULL;
    for (int i = 0; dirs[i]; i++)
    {
        char *full = ft_strjoin(dirs[i], "/");
        char *full_cmd = ft_strjoin(full, cmd);
        free(full);
        if (access(full_cmd, X_OK) == 0)
        {
            ft_split_free(dirs);
            return full_cmd;
        }
        free(full_cmd);
    }
    ft_split_free(dirs);
    errno = ENOENT;
    return NULL;
}

char *resolve_binary(char *cmd)
{
    struct stat st;
    if (strchr(cmd, '/') != NULL)
    {
        if (access(cmd, F_OK) == -1)
            errno = ENOENT;
        else if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
            errno = EISDIR;
        else if (access(cmd, X_OK) == -1)
            errno = EACCES;
        else
            return (ft_strdup(cmd));
        return (NULL);
    }
    else
        return (resolve_from_path_env(cmd));
}

char **convert_arguments(t_list *args, t_executor_ctx *ctx)
{
    char **argv = ft_calloc(ft_lstsize(args) + 1, sizeof(char *));
    int i = 0;

    for (t_list *node = args; node; node = node->next) {
        t_word *word = node->content;
        char buffer[1024] = {0};

        for (t_list *seg = word->segments; seg; seg = seg->next) {
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

            ft_strcat(buffer, resolved);
            if (s->type == EXIT_STATUS) free(resolved); // Cleanup itoa
        }
	    if (buffer[0] != '\0')
            argv[i++] = ft_strdup(buffer);
    }

    return argv;
}

int handle_cd(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);
    int ret = 0;

    if (setup_redirections(redirects, ctx) < 0)
    {
        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        dup2(save_stderr, STDERR_FILENO);
        close(save_stdin);
        close(save_stdout);
        close(save_stderr);
        ctx->last_exit_status = 1;
        return 1;
    }
    if (!argv[1]) {
        char *home = getenv("HOME");
        if (!home) {
            fprintf(stderr, "cd: HOME not set\n");
            ret = 1;
        } else {
            if (chdir(home) != 0) {
                perror("cd");
                ret = 1;
            }
        }
    } else if (argv[2]) {
        fprintf(stderr, "cd: too many arguments\n");
        ret = 1;
    } else {
        if (chdir(argv[1]) != 0) {
            perror("cd");
            ret = 1;
        }
    }
    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);
    dup2(save_stderr, STDERR_FILENO);
    close(save_stdin);
    close(save_stdout);
    close(save_stderr);
    ctx->last_exit_status = ret;
    return ret;
}

int execute_exit(char **argv, t_executor_ctx *ctx)
{
    int exit_code = ctx->last_exit_status;
    int arg_count = 0;

    while (argv[arg_count]) arg_count++;
    arg_count--;
    if (arg_count > 1) {
        ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
        exit_code = 1;
    }
    else if (arg_count == 1) {
        if (!is_valid_integer(argv[1])) {
            ft_putstr_fd("minishell: exit: numeric argument required\n", STDERR_FILENO);
            exit_code = 2;
        } else {
            exit_code = ft_atoi(argv[1]);
            exit_code = exit_code % 256;
            if (exit_code < 0) exit_code += 256;
        }
    }
    exit(exit_code);
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

void execute_child(t_command *cmd, t_executor_ctx *ctx)
{
    char **argv = convert_arguments(cmd->arguments, ctx);
    int status = 0;
    
    if (!argv || !argv[0] || argv[0][0] == '\0') {
        ft_split_free(argv);
        exit(status);
    }
    if (argv && argv[0] && is_builtin(argv[0])) {
        status = execute_builtin(argv, cmd->redirects, ctx);
        ft_split_free(argv);
        exit(status);
    }
    char *path = resolve_binary(argv[0]);
    if (!path)
    {
        if (errno == EACCES)
        {
            fprintf(stderr, "minishell: %s: Permission denied\n", argv[0]);
            ft_split_free(argv);
            exit(126);
        }
        else if (errno == EISDIR)
        {
            fprintf(stderr, "minishell: %s: Is a directory\n", argv[0]);
            ft_split_free(argv);
            exit(126);
        }
        else if (errno == ENOENT)
        {
            if (strchr(argv[0], '/') != NULL)
                fprintf(stderr, "minishell: %s: No such file or directory\n", argv[0]);
            else
                fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
            ft_split_free(argv);
            exit(127);
        }
        else
        {
            fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
            ft_split_free(argv);
            exit(127);
        }
    }
    execve(path, argv, environ);
    perror("execve");
    exit(126);
}

void execute_pipeline_commands(t_pipeline *pipeline, t_executor_ctx *ctx)
{
    int prev_fd = -1;
    pid_t last_pid = -1;

    for (t_list *node = pipeline->commands; node; node = node->next) {
        t_command *cmd = node->content;
        int is_last = (node->next == NULL);
        int pipe_fd[2];

        if (!is_last && pipe(pipe_fd) < 0) {
            perror("pipe");
            ctx->last_exit_status = 1;
            return;
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (!is_last) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
            }
            if (setup_redirections(cmd->redirects, ctx) < 0) exit(1);
            execute_child(cmd, ctx);
        } else if (pid < 0) {
            perror("fork");
            ctx->last_exit_status = 1;
            return;
        }
        if (prev_fd != -1) close(prev_fd);
        if (!is_last) {
            close(pipe_fd[1]);
            prev_fd = pipe_fd[0];
        }
        last_pid = pid;
    }
    int status;
    waitpid(last_pid, &status, 0);
    if (WIFEXITED(status)) ctx->last_exit_status = WEXITSTATUS(status);
    while (wait(NULL) > 0);
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

