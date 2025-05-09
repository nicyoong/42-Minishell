#include "parser.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include "executor.h"
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

// Utilities
void ft_split_free(char **arr)
{
    if (!arr)
        return;
    for (int i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}

int process_heredoc(t_word *delimiter_word)
{
    int fds[2];
    pipe(fds);
    char *delim = NULL;
    char buffer[1024] = {0};
    for (t_list *seg = delimiter_word->segments; seg; seg = seg->next)
        strcat(buffer, ((t_segment *)seg->content)->value);
    delim = buffer;
    while (1)
    {
        char *line = readline("> ");
        if (!line || strcmp(line, delim) == 0)
            break;
        write(fds[1], line, strlen(line));
        write(fds[1], "\n", 1);
        free(line);
    }
    close(fds[1]);
    return (fds[0]);
}

char *resolve_segment(t_segment *seg, t_executor_ctx *ctx) {
    if (seg->type == VARIABLE) {
        char *value = getenv(seg->value);
        return value ? ft_strdup(value) : ft_strdup("");
    } else if (seg->type == EXIT_STATUS) {
        return ft_itoa(ctx->last_exit_status);
    }
    return ft_strdup(seg->value);
}

int setup_redirections(t_list *redirects)
{
    for (t_list *node = redirects; node; node = node->next) 
    {
        t_redirect *r = node->content;
        char path[1024] = {0};
        for (t_list *seg = r->filename->segments; seg; seg = seg->next)
            strcat(path, ((t_segment *)seg->content)->value);
        int fd = -1;
        if (r->type == REDIR_IN)
            fd = open(path, O_RDONLY);
        else if (r->type == REDIR_OUT)
            fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else if (r->type == REDIR_APPEND)
            fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else if (r->type == REDIR_HEREDOC)
            fd = process_heredoc(r->filename);
        if (fd < 0)
        {
            perror("redirection error");
            return -1;
        }
        int target = (r->type == REDIR_IN || r->type == REDIR_HEREDOC) ? STDIN_FILENO : STDOUT_FILENO;
        if (dup2(fd, target) < 0)
        {
            perror("dup2 error");
            close(fd);
            return -1;
        }
        close(fd);
    }
    return 0;
}

// void execute_export(char **argv, t_list *redirects)
// {
//     int save_stdin = dup(STDIN_FILENO);
//     int save_stdout = dup(STDOUT_FILENO);
//     int save_stderr = dup(STDERR_FILENO);

//     if (setup_redirections(redirects) < 0)
//     {
//         // restore
//         dup2(save_stdin, STDIN_FILENO);
//         dup2(save_stdout, STDOUT_FILENO);
//         dup2(save_stderr, STDERR_FILENO);
//         close(save_stdin);
//         close(save_stdout);
//         close(save_stderr);
//         return;
//     }

//     int i = 1;
//     if (!argv[1])
//     {
//         char **env = environ;
//         while (*env)
//         {
//             printf("%s\n", *env);
//             env++;
//         }
//     }
//     else
//     {
//         while (argv[i])
//         {
//             if (putenv(argv[i]) != 0)
//                 perror("export");
//             i++;
//         }
//     }

//     dup2(save_stdin, STDIN_FILENO);
//     dup2(save_stdout, STDOUT_FILENO);
//     dup2(save_stderr, STDERR_FILENO);
//     close(save_stdin);
//     close(save_stdout);
//     close(save_stderr);
// }

void execute_export(char **argv, t_list *redirects)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);

    if (setup_redirections(redirects) < 0)
    {
        // restore
        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        dup2(save_stderr, STDERR_FILENO);
        close(save_stdin);
        close(save_stdout);
        close(save_stderr);
        return;
    }

    int i = 1;
    if (!argv[1])
    {
        char **env = environ;
        while (*env)
        {
            printf("%s\n", *env);
            env++;
        }
    }
    else
    {
        while (argv[i])
        {
            if (putenv(argv[i]) != 0)
                perror("export");
            i++;
        }
    }

    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);
    dup2(save_stderr, STDERR_FILENO);
    close(save_stdin);
    close(save_stdout);
    close(save_stderr);
}

void	execute_unset(char **argv, t_list *redirects)
{
	int i = 1;

	while (argv[i])
	{
		if (unsetenv(argv[i]) != 0)
			perror("unset");
		i++;
	}
}

char *resolve_binary(char *cmd)
{
    if (access(cmd, X_OK) == 0)
        return ft_strdup(cmd);
    char *path = getenv("PATH");
    if (!path)
        return (NULL);
    char **dirs = ft_split(path, ':');
    for (int i = 0; dirs[i]; i++)
    {
        char *full = ft_strjoin(dirs[i], "/");
        char *full_cmd = ft_strjoin(full, cmd);
        free(full);
        if (access(full_cmd, X_OK) == 0)
        {
            ft_split_free(dirs);
            return (full_cmd);
        }
        free(full_cmd);
    }
    ft_split_free(dirs);
    return (NULL);
}

char **convert_arguments(t_list *args, t_executor_ctx *ctx)
{
    int count = ft_lstsize(args);
    char **argv = ft_calloc(count + 1, sizeof(char *));
    int i = 0;
    
    for (t_list *node = args; node; node = node->next) {
        t_word *word = node->content;
        char buffer[1024] = {0};
        
        for (t_list *seg = word->segments; seg; seg = seg->next) {
            t_segment *s = seg->content;
            char *value = NULL;
            
            if (s->type == VARIABLE) {
                value = getenv(s->value);
                if (!value) value = "";
            }
            else if (s->type == EXIT_STATUS) {
                value = ft_itoa(ctx->last_exit_status);
            }
            else { // LITERAL
                value = s->value;
            }
            
            strcat(buffer, value);
            if (s->type == EXIT_STATUS) free(value);
        }
        argv[i++] = ft_strdup(buffer);
    }
    return (argv);
}

int handle_cd(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);
    int ret = 0;

    if (setup_redirections(redirects) < 0)
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
        char *home = getenv("HOME");
        if (!home)
        {
            fprintf(stderr, "cd: HOME not set\n");
            ret = 1;
        }
        else
            ret = chdir(home) == 0 ? 0 : 1;
    }
    else if (argv[2])
    {
        fprintf(stderr, "cd: too many arguments\n");
        ret = 1;
    }
    else
        ret = chdir(argv[1]) == 0 ? 0 : 1;

    if (ret != 0)
        perror("cd");
    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);
    dup2(save_stderr, STDERR_FILENO);
    close(save_stdin);
    close(save_stdout);
    close(save_stderr);\
    ctx->last_exit_status = ret;
    return ret;
}

// Main execution
void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx)
{
    int status;
    pid_t last_pid = -1;

    if (ft_lstsize(pipeline->commands) == 1) {
        t_command *cmd = pipeline->commands->content;
        char **argv = convert_arguments(cmd->arguments, ctx);
        
        // Built-in commands
        if (argv && argv[0]) {
            if (strcmp(argv[0], "cd") == 0) {
                handle_cd(argv, cmd->redirects, ctx);
                ctx->last_exit_status = 0; // Update based on actual success
                ft_split_free(argv);
                return;
            }
            else if (strcmp(argv[0], "export") == 0) {
                execute_export(argv, cmd->redirects);
                ctx->last_exit_status = 0;
                ft_split_free(argv);
                return;
            }
            else if (strcmp(argv[0], "unset") == 0) {
                execute_unset(argv, cmd->redirects);
                ctx->last_exit_status = 0;
                ft_split_free(argv);
                return;
            }
        }
        ft_split_free(argv);
    }

    int prev_fd = -1;
    int pipe_fd[2];
    t_list *node = pipeline->commands;

    while (node) {
        t_command *cmd = node->content;
        int is_last = (node->next == NULL);

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
            if (setup_redirections(cmd->redirects) < 0)
                exit(1);
            char **argv = convert_arguments(cmd->arguments, ctx);
            char *path = resolve_binary(argv[0]);
            
            if (!path) {
                fprintf(stderr, "Command not found: %s\n", argv[0]);
                ft_split_free(argv);
                exit(127);
            }
            
            execve(path, argv, environ);
            perror("execve");
            exit(127);
        }
        else if (pid < 0) {
            perror("fork");
            ctx->last_exit_status = 1;
            return;
        }

        // Parent process
        if (prev_fd != -1) close(prev_fd);
        if (!is_last) {
            close(pipe_fd[1]);
            prev_fd = pipe_fd[0];
        }
        
        last_pid = pid;
        node = node->next;
    }

    // Wait for last command and capture exit status
    waitpid(last_pid, &status, 0);
    if (WIFEXITED(status)) {
        ctx->last_exit_status = WEXITSTATUS(status);
    }
    
    // Cleanup remaining processes
    while (wait(NULL) > 0);
}
