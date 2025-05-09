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
#include "utils.h"

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

int is_builtin(const char *cmd) {
    return (cmd && (ft_strcmp(cmd, "cd") == 0 || 
                   ft_strcmp(cmd, "export") == 0 || 
                   ft_strcmp(cmd, "unset") == 0));
}

int process_heredoc(t_word *delimiter_word, t_executor_ctx *ctx)
{
    int fds[2];
    pipe(fds);
    
    char buffer[1024] = {0};
    for (t_list *seg = delimiter_word->segments; seg; seg = seg->next) {
        t_segment *s = seg->content;
        char *resolved = NULL;

        if (s->type == VARIABLE) {
            resolved = getenv(s->value);
            if (!resolved) resolved = "";
        } 
        else if (s->type == EXIT_STATUS) {
            resolved = ft_itoa(ctx->last_exit_status);
        } 
        else {
            resolved = s->value;
        }
        
        strcat(buffer, resolved);
        
        if (s->type == EXIT_STATUS) {
            free(resolved);
        }
    }
    char *delim = buffer;

    char *line;
    while (1) {
        line = readline("> ");
        if (!line || strcmp(line, delim) == 0) break;
        write(fds[1], line, strlen(line));
        write(fds[1], "\n", 1);
        free(line);
    }
    close(fds[1]);
    return fds[0];
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

int setup_redirections(t_list *redirects, t_executor_ctx *ctx)
{
    for (t_list *node = redirects; node; node = node->next) 
    {
        t_redirect *r = node->content;
        char path[1024] = {0};

        // Resolve filename segments (variables and exit status)
        for (t_list *seg = r->filename->segments; seg; seg = seg->next) {
            t_segment *s = seg->content;
            char *resolved = NULL;
            
            if (s->type == VARIABLE) {
                resolved = getenv(s->value);
                if (!resolved) resolved = "";
            } 
            else if (s->type == EXIT_STATUS) {
                resolved = ft_itoa(ctx->last_exit_status);
            } 
            else { // LITERAL
                resolved = s->value;
            }
            
            strcat(path, resolved);
            
            // Cleanup dynamically allocated exit status
            if (s->type == EXIT_STATUS) {
                free(resolved);
            }
        }

        int fd = -1;
        int flags = 0;
        mode_t mode = 0644;

        switch (r->type) {
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
                fd = process_heredoc(r->filename, ctx);  // Updated heredoc handler
                break;
            default:
                fprintf(stderr, "Unknown redirection type\n");
                return -1;
        }

        if (fd < 0) {
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

int execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);
    int ret = 0;  // Track overall success (0) vs failure (1)

    if (setup_redirections(redirects, ctx) < 0)
    {
        // Restore and cleanup
        dup2(save_stdin, STDIN_FILENO);
        dup2(save_stdout, STDOUT_FILENO);
        dup2(save_stderr, STDERR_FILENO);
        close(save_stdin);
        close(save_stdout);
        close(save_stderr);
        ctx->last_exit_status = 1;
        return 1;  // Explicit return
    }

    if (!argv[1]) {
        // Print environment (always succeeds)
        extern char **environ;
        for (char **env = environ; *env; env++) {
            printf("%s\n", *env);
        }
    } else {
        for (int i = 1; argv[i]; i++) {
            char *arg = argv[i];
            char *eq = strchr(arg, '=');
            
            if (eq) {
                *eq = '\0';
                char *name = arg;
                char *value = eq + 1;
                
                if (setenv(name, value, 1) != 0) {
                    perror("export");
                    ret = 1;  // Mark failure
                }
            } else {
                char *current = getenv(arg);
                if (setenv(arg, current ? current : "", 1) != 0) {
                    perror("export");
                    ret = 1;  // Mark failure
                }
            }
        }
    }

    // Restore streams
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
    int ret = 0;
    int i = 1;

    while (argv[i]) {
        if (unsetenv(argv[i]) != 0) {
            perror("unset");
            ret = 1;
        }
        i++;
    }
    ctx->last_exit_status = ret;
    return ret;
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
    char **argv = ft_calloc(ft_lstsize(args) + 1, sizeof(char *));
    int i = 0;

    for (t_list *node = args; node; node = node->next) {
        t_word *word = node->content;
        char buffer[1024] = {0};

        for (t_list *seg = word->segments; seg; seg = seg->next) {
            t_segment *s = seg->content;
            char *resolved = NULL;

            if (s->type == VARIABLE) {
                resolved = getenv(s->value);  // Get value from environment
                if (!resolved) resolved = ""; // Handle unset variables
            } else if (s->type == EXIT_STATUS) {
                resolved = ft_itoa(ctx->last_exit_status);  // Resolve $?
            } else {
                resolved = s->value;  // Literal
            }

            strcat(buffer, resolved);
            if (s->type == EXIT_STATUS) free(resolved); // Cleanup itoa
        }

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


int execute_builtin(char **argv, t_list *redirects, t_executor_ctx *ctx) {
    if (ft_strcmp(argv[0], "cd") == 0) {
        return handle_cd(argv, redirects, ctx);
    } else if (ft_strcmp(argv[0], "export") == 0) {
        return execute_export(argv, redirects, ctx);
    } else if (ft_strcmp(argv[0], "unset") == 0) {
        return execute_unset(argv, redirects, ctx);
    }
    return -1; // Not a builtin
}

// Main execution
// void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx)
// {
//     int status;
//     pid_t last_pid = -1;

//     if (ft_lstsize(pipeline->commands) == 1) {
//         t_command *cmd = pipeline->commands->content;
//         char **argv = convert_arguments(cmd->arguments, ctx);
        
//         // Built-in commands
//         if (argv && argv[0]) {
//             if (strcmp(argv[0], "cd") == 0) {
//                 handle_cd(argv, cmd->redirects, ctx);
//                 ctx->last_exit_status = 0; // Update based on actual success
//                 ft_split_free(argv);
//                 return;
//             }
//             else if (strcmp(argv[0], "export") == 0) {
//                 execute_export(argv, cmd->redirects, ctx);
//                 ft_split_free(argv);
//                 return;
//             }
//             else if (strcmp(argv[0], "unset") == 0) {
//                 execute_unset(argv, cmd->redirects);
//                 ctx->last_exit_status = 0;
//                 ft_split_free(argv);
//                 return;
//             }
//         }
//         ft_split_free(argv);
//     }

//     int prev_fd = -1;
//     int pipe_fd[2];
//     t_list *node = pipeline->commands;

//     while (node) {
//         t_command *cmd = node->content;
//         int is_last = (node->next == NULL);

//         if (!is_last && pipe(pipe_fd) < 0) {
//             perror("pipe");
//             ctx->last_exit_status = 1;
//             return;
//         }

//         pid_t pid = fork();
//         if (pid == 0) {
//             if (prev_fd != -1) {
//                 dup2(prev_fd, STDIN_FILENO);
//                 close(prev_fd);
//             }
//             if (!is_last) {
//                 close(pipe_fd[0]);
//                 dup2(pipe_fd[1], STDOUT_FILENO);
//                 close(pipe_fd[1]);
//             }
//             if (setup_redirections(cmd->redirects, ctx) < 0)
//                 exit(1);
//             char **argv = convert_arguments(cmd->arguments, ctx);
//             char *path = resolve_binary(argv[0]);
            
//             if (!path) {
//                 fprintf(stderr, "Command not found: %s\n", argv[0]);
//                 ft_split_free(argv);
//                 exit(127);
//             }
            
//             execve(path, argv, environ);
//             perror("execve");
//             exit(127);
//         }
//         else if (pid < 0) {
//             perror("fork");
//             ctx->last_exit_status = 1;
//             return;
//         }

//         // Parent process
//         if (prev_fd != -1) close(prev_fd);
//         if (!is_last) {
//             close(pipe_fd[1]);
//             prev_fd = pipe_fd[0];
//         }
        
//         last_pid = pid;
//         node = node->next;
//     }

//     // Wait for last command and capture exit status
//     waitpid(last_pid, &status, 0);
//     if (WIFEXITED(status)) {
//         ctx->last_exit_status = WEXITSTATUS(status);
//     }
    
//     // Cleanup remaining processes
//     while (wait(NULL) > 0);
// }

void execute_child(t_command *cmd, t_executor_ctx *ctx)
{
    char **argv = convert_arguments(cmd->arguments, ctx);
    
    // Check for builtin first
    if (argv && argv[0] && is_builtin(argv[0])) {
        int status = execute_builtin(argv, cmd->redirects, ctx);
        ft_split_free(argv);
        exit(status);
    }
    
    // Handle external commands
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
            // Child process
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

        // Parent cleanup
        if (prev_fd != -1) close(prev_fd);
        if (!is_last) {
            close(pipe_fd[1]);
            prev_fd = pipe_fd[0];
        }
        last_pid = pid;
    }

    // Wait for completion
    int status;
    waitpid(last_pid, &status, 0);
    if (WIFEXITED(status)) ctx->last_exit_status = WEXITSTATUS(status);
    while (wait(NULL) > 0);
}	

void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx)
{
    // Handle single builtin commands
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
    
    // Handle pipelines and external commands
    execute_pipeline_commands(pipeline, ctx);
}

