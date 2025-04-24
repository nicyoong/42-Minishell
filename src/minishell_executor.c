

#include "parser.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>

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

char **convert_arguments(t_list *args) 
{
    int count = ft_lstsize(args);
    char **argv = ft_calloc(count + 1, sizeof(char *));
    int i = 0;
    for (t_list *node = args; node; node = node->next)
    {
        t_word *word = node->content;
        char buffer[1024] = {0};
        for (t_list *seg = word->segments; seg; seg = seg->next)
        {
            t_segment *s = seg->content;
            strcat(buffer, s->value);  // Variable expansion assumed done earlier
        }
        argv[i++] = ft_strdup(buffer);
    }
    return (argv);
}

int process_heredoc(t_word *delimiter_word) 
{
    int fds[2];
    pipe(fds);
    char *delim = NULL;
    char buffer[PATH_MAX] = {0};

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

void setup_redirections(t_list *redirects) 
{
    for (t_list *node = redirects; node; node = node->next) 
    {
        t_redirect *r = node->content;
        char path[PATH_MAX] = {0};
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
            exit(1);
        }
        int target = (r->type == REDIR_IN || r->type == REDIR_HEREDOC) ? STDIN_FILENO : STDOUT_FILENO; // to revisit this.
        dup2(fd, target);
        close(fd);
    }
}

// Main execution
void execute_pipeline(t_pipeline *pipeline)
{
    int prev_fd = -1;
    int pipe_fd[2];

    for (t_list *node = pipeline->commands; node; node = node->next)
    {
        t_command *cmd = node->content;
        int is_last = (node->next == NULL);
        if (!is_last && pipe(pipe_fd) < 0)
        {
            perror("pipe");
            return;
        }
        pid_t pid = fork();
        if (pid == 0)
        {
            // --- CHILD ---
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
            setup_redirections(cmd->redirects);
            char **argv = convert_arguments(cmd->arguments);
            char *path = resolve_binary(argv[0]);
            if (!path) 
            {
                printf("Command not found: %s\n", argv[0]);
                exit(127);
            }
            execve(path, argv, environ);
            perror("execve"); exit(127);
        } 
        else if (pid < 0) 
        {
            perror("fork");
            return;
        }
        if (prev_fd != -1) close(prev_fd);
        if (!is_last)
        {
            close(pipe_fd[1]);
            prev_fd = pipe_fd[0];
        }
    }
    while (wait(NULL) > 0);
}
