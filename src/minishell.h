#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <ctype.h>
# include "../libft/libft.h"
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <limits.h>

int ft_strcmp(const char *s1, const char *s2);
void ft_split_free(char **array);
int  is_builtin(const char *cmd);
int is_whitespace(char c);
int is_operator_char(char c);
int is_valid_var_char(char c);
int is_valid_identifier(const char *str);
int is_valid_integer(const char *str);
char *ft_strcat(char *dest, const char *src);
char *ft_strncpy(char *dest, const char *src, size_t n);

typedef enum e_token_type {
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_APPEND,
	TOKEN_REDIRECT_HEREDOC
}	t_token_type;

typedef enum e_segment_type {
	LITERAL,
	VARIABLE,
	EXIT_STATUS
}	t_segment_type;

typedef enum e_redirect_type {
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redirect_type;

typedef struct s_segment {
	t_segment_type	type;
	char			*value;
}	t_segment;

typedef struct s_word {
	t_list			*segments;
}	t_word;

typedef struct s_token {
	t_token_type	type;
	t_word			*word;
}	t_token;

typedef struct s_redirect {
	t_redirect_type	type;
	t_word			*filename;  // Changed from char* to t_word*
}	t_redirect;

typedef struct s_command {
	t_list			*arguments;  // Added missing members
	t_list			*redirects;
}	t_command;

typedef struct s_pipeline {
	t_list			*commands;
}	t_pipeline;

// lexer.c
// Token functions
t_token *create_token(t_token_type type);
void add_segment(t_word *word, t_segment_type seg_type, const char *value);
void free_token(void *token_ptr);

// Lexer function
t_list *lex_input(const char *input);

// parser.c
// Helper functions
int is_redirect(t_token_type type);
t_redirect_type token_to_redirect(t_token_type type);
t_word *copy_word(t_word *src);
void free_word(void *word_ptr);
void free_redirect(void *redir_ptr);
void free_command(void *cmd_ptr);
void free_segment(void *seg_ptr);

// Main parsing functions
t_command *parse_command(t_list **tokens);
t_list *split_commands(t_list *tokens);
t_pipeline *parse(t_list *tokens);
void free_pipeline(t_pipeline *pipeline);

// Utility/printing functions
void print_word(t_word *word);
void print_pipeline(t_pipeline *pipeline);

// execution

typedef struct s_executor_ctx {
    int last_exit_status;
} t_executor_ctx;

void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx);
void    sigint_handler(int signo);
void    setup_signal_handlers(void);

//process heredoc
int	process_heredoc(t_word *delimiter_word, t_executor_ctx *ctx);


//builtin function
int     execute_pwd(char **argv, t_list *redirects, t_executor_ctx *ctx);
int	execute_echo(char **argv, t_list *redirects, t_executor_ctx *ctx);
int	execute_env(char **argv, t_list *redirects, t_executor_ctx *ctx);
int	execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx);
int	execute_unset(char **argv, t_list *redirects, t_executor_ctx *ctx);
int	handle_cd(char **argv, t_list *redirects, t_executor_ctx *ctx);
int	execute_exit(char **argv, t_executor_ctx *ctx);
int	execute_builtin(char **argv, t_list *redirects, t_executor_ctx *ctx);

//to be sorted
char	*resolve_segment(t_segment *seg, t_executor_ctx *ctx);
int	setup_redirections(t_list *redirects, t_executor_ctx *ctx);
void	cleanup_redirections(int save_stdin, int save_stdout, int save_stderr, t_executor_ctx *ctx, int ret);
char	*resolve_from_path_env(char *cmd);
char	*resolve_binary(char *cmd);
char	**convert_arguments(t_list *args, t_executor_ctx *ctx);
void	execute_child(t_command *cmd, t_executor_ctx *ctx);
void	execute_pipeline_commands(t_pipeline *pipeline, t_executor_ctx *ctx);
void	execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx);


# endif
