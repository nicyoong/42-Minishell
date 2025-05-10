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

void init_env_vars(t_executor_ctx *ctx, char **environ) {
    ctx->env_vars = NULL;
    for (char **env = environ; *env; env++) {
        char *eq = strchr(*env, '=');
        if (!eq) continue;
        char *name = strndup(*env, eq - *env);
        char *value = strdup(eq + 1);
        t_var *var = malloc(sizeof(t_var));
        var->name = name;
        var->value = value;
        var->exported = true;
        ft_lstadd_back(&ctx->env_vars, ft_lstnew(var));
    }
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

t_var *find_var(t_list *env_vars, const char *name)
{
    for (t_list *node = env_vars; node; node = node->next) {
        t_var *var = node->content;
        if (ft_strcmp(var->name, name) == 0) {
            return var;
        }
    }
    return NULL;
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

    if (!argv[1]) {
        // Print only exported variables from custom environment list
        for (t_list *node = ctx->env_vars; node; node = node->next) {
            t_var *var = node->content;
            if (var->exported) {
                printf("export %s=\"%s\"\n", var->name, var->value);
            }
        }
    } else {
        for (int i = 1; argv[i]; i++) {
            char *arg = argv[i];
            char *eq = strchr(arg, '=');
            char *name = NULL;
            char *error_part = NULL;
            int name_invalid = 0;

            // Extract name and validate
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
                continue;
            }

            // Find existing variable or create new
            t_var *existing = find_var(ctx->env_vars, name);
            if (existing) {
                if (eq) {  // Update value if assignment
                    free(existing->value);
                    existing->value = ft_strdup(eq + 1);
                }
                existing->exported = true;
            } else {
                t_var *new_var = malloc(sizeof(t_var));
                new_var->name = ft_strdup(name);
                new_var->value = eq ? ft_strdup(eq + 1) : ft_strdup("");
                new_var->exported = true;
                ft_lstadd_back(&ctx->env_vars, ft_lstnew(new_var));
            }

            free(name);
        }
    }

    // Restore standard streams
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
    for (int i = 1; argv[i]; i++) {
        t_list **prev_next = &ctx->env_vars;
        for (t_list *node = ctx->env_vars; node; node = node->next) {
            t_var *var = node->content;
            if (strcmp(var->name, argv[i]) == 0) {
                *prev_next = node->next;
                free(var->name);
                free(var->value);
                free(var);
                free(node);
                break;
            }
            prev_next = &node->next;
        }
    }
    return 0;
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
    else if (argv[1] && argv[2])
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
    close(save_stderr);
    ctx->last_exit_status = ret;
    return ret;
}

int execute_pwd(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	(void)argv;
  
	int save_stdout = dup(STDOUT_FILENO);
	char cwd[PATH_MAX];
  
	if (setup_redirections(redirects, ctx) < 0) {
	  dup2(save_stdout, STDOUT_FILENO);
	  close(save_stdout);
	  ctx->last_exit_status = 1;
	  return 1;
	}
	if (getcwd(cwd, sizeof(cwd))) {
	  printf("%s\n", cwd);
	  ctx->last_exit_status = 0;
	} else {
	  perror("pwd");
	  ctx->last_exit_status = 1;
	}
	dup2(save_stdout, STDOUT_FILENO);
	close(save_stdout);
	return ctx->last_exit_status;
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
	}
    return -1;
}

void execute_child(t_command *cmd, t_executor_ctx *ctx)
{
    char **argv = convert_arguments(cmd->arguments, ctx);

    if (argv && argv[0] && is_builtin(argv[0])) {
        int status = execute_builtin(argv, cmd->redirects, ctx);
        ft_split_free(argv);
        exit(status);
    }
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

