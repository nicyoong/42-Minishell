/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 08:15:13 by tching            #+#    #+#             */
/*   Updated: 2025/06/11 02:43:31 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <ctype.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <limits.h>

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_APPEND,
	TOKEN_REDIRECT_HEREDOC,
	TOKEN_HERESTRING
}	t_token_type;

typedef enum e_segment_type
{
	LITERAL,
	VARIABLE,
	EXIT_STATUS
}	t_segment_type;

typedef enum e_redirect_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC,
	REDIR_HERESTRING
}	t_redirect_type;

typedef struct s_segment
{
	t_segment_type	type;
	char			*value;
}	t_segment;

typedef struct s_word
{
	t_list			*segments;
}	t_word;

typedef struct s_token
{
	t_token_type	type;
	t_word			*word;
}	t_token;

typedef struct s_redirect
{
	t_redirect_type	type;
	t_word			*filename;
}	t_redirect;

typedef struct s_command
{
	t_list			*arguments;
	t_list			*redirects;

	int				heredoc_fd; //#NEW#
}	t_command;

typedef struct s_pipeline
{
	t_list			*commands;
}	t_pipeline;

typedef struct s_export
{
	char					*name;
	bool					assigned;

	struct s_export			*next;

}	t_export;

typedef struct s_pipe_info
{
	int	prev_fd;
	int	pipe_fd[2];
	int	is_last;
}	t_pipe_info;

typedef struct s_executor_ctx
{
	int			last_exit_status;
	t_export	*export_list;
}	t_executor_ctx;

// lexer.c
// Token functions
t_token			*create_token(t_token_type type);
void			add_segment(t_word *word,
					t_segment_type seg_type, const char *value);
void			free_token(void *token_ptr);

// Lexer function
void			flush_buffer(t_word *word, char *buffer, int *buf_idx);
void			parse_exit_status(const char *input, int *i, t_word *word);
void			parse_variable_name(const char *input, int *i, t_word *word);
void			prepare_for_expansion(t_word *word, char *buffer,
					int *buf_idx, int *i);
void			expand_variable(const char *input, int *i, t_word *word);
int				process_quoted_content(const char *input, int *i,
					char quote_type, t_word *word);
char			decode_newline(int *idx);
char			decode_tab(int *idx);
char			decode_backslash(int *idx);
int				is_hex_escape(const char *s, int idx);
char			decode_hex(const char *s, int *idx);
char			decode_quote(int *idx);
char			decode_default(const char *s, int *idx);
char			decode_escape(const char *s, int *idx);
void			process_ansi_c_quote(const char *input, int *i, t_word *word);
void			process_unquoted_segment(const char *input,
					int *i, t_word *word);
t_list			*lex_input(const char *input);
t_token_type	get_operator(const char *input, int *i);

// parser.c
t_word			*copy_word(t_word *src);
int				is_redirect(t_token_type type);
t_redirect_type	token_to_redirect(t_token_type type);
int				init_redirect(t_list **tokens, t_redirect **redir);
int				handle_redirect(t_list **tokens, t_command *cmd);
int				process_redirect(t_list **tokens, t_command *cmd, t_list *head);
int				process_word(t_list **tokens, t_command *cmd, t_list *head);
int				process_token(t_list **tokens, t_command *cmd);
t_command		*parse_command(t_list **tokens);
void			clear_token_list(void *content);
void			add_token_to_current(t_list **current, t_token *token);
int				finalize_current_command(t_list **cmds, t_list **current);
t_list			*clear_on_error(t_list **current, t_list **cmds);
int				finalize_command(t_list **cmds, t_list **current);
t_list			*split_commands(t_list *tokens);
int				parse_command_group(t_list *curr_group, t_pipeline *pipeline);
t_pipeline		*parse(t_list *tokens);

// Helper functions
t_redirect_type	token_to_redirect(t_token_type type);
int				is_redirect(t_token_type type);
void			free_word(void *word_ptr);
void			free_redirect(void *redir_ptr);
void			free_command(void *cmd_ptr);
void			free_segment(void *seg_ptr);
t_word			*copy_word(t_word *src);

// Main parsing functions
t_command		*parse_command(t_list **tokens);
t_list			*split_commands(t_list *tokens);
char			*token_type_to_str(t_token_type type);
int				print_syntax_error_token(char *tok);
int				check_redirect_end(t_token *current, t_token *next);
int				check_redirect_redirect(t_token *current, t_token *next);
int				check_redirect_pipe(t_token *current, t_token *next);
int				check_pipe_end(t_token *current, t_token *next);
int				check_pipe_follow(t_token *current, t_token *next);
int				validate_syntax(t_list *tokens);
int				handle_syntax_error(t_list *tokens,
					char *full_line, t_executor_ctx *ctx);

t_pipeline		*parse(t_list *tokens);

void			free_pipeline(t_pipeline *pipeline);
void			free_export_list(t_export *head);
int				handle_pipeline_failure(t_pipeline *pipeline,
					t_list *tokens, char *full_line);

// execution
void			execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx);
void			sigint_handler(int signo);
void			set_executor_ctx(t_executor_ctx *ctx);
void			setup_signal_handlers(void);

//process heredoc
int				process_heredoc(t_word *delimiter_word, t_executor_ctx *ctx);

//builtin function
int				execute_pwd(char **argv,
					t_list *redirects, t_executor_ctx *ctx);
int				execute_echo(char **argv,
					t_list *redirects, t_executor_ctx *ctx);
int				execute_env(char **argv,
					t_list *redirects, t_executor_ctx *ctx);
int				execute_export(char **argv,
					t_list *redirects, t_executor_ctx *ctx);
int				execute_unset(char **argv,
					t_list *redirects, t_executor_ctx *ctx);
int				handle_cd(char **argv,
					t_list *redirects, t_executor_ctx *ctx);
int				execute_exit(char **argv, t_executor_ctx *ctx);
int				execute_builtin(char **argv,
					t_list *redirects, t_executor_ctx *ctx);
void			handle_builtin_command(char **argv,
					t_command *cmd, t_executor_ctx *ctx);
void			handle_invalid_arguments(char **argv);
void			handle_path_errors(char *path, char **argv);

//redirections
int				setup_redirections(t_list *redirects, t_executor_ctx *ctx);
int				build_path_from_word(t_word *word, char *buffer,
					size_t bufsize, t_executor_ctx *ctx);
int				open_redirection_fd(t_redirect_type type, const char *path,
					t_word *filename, t_executor_ctx *ctx);
char			*trim_and_validate_path(const char *path);
char			*resolve_segment_value(t_segment *s, t_executor_ctx *ctx);
void			cleanup_redirections(int save_stdin,
					int save_stdout, int save_stderr);
int				redirect_path_msg(void);
int				redirect_invalid_msg(void);

//to be sorted
char			*resolve_segment(t_segment *seg, t_executor_ctx *ctx);
char			*resolve_from_path_env(char *cmd);
char			*resolve_binary(char *cmd);
char			**convert_arguments(t_list *args, t_executor_ctx *ctx);
void			execute_child(t_command *cmd, t_executor_ctx *ctx);
void			execute_pipeline_commands(t_pipeline *pipeline,
					t_executor_ctx *ctx);
void			execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx);
void			remove_export(t_executor_ctx *ctx, const char *name);
void			setup_child_process(t_command *cmd,
					t_pipe_info *pinfo, t_executor_ctx *ctx);
void			wait_for_children(pid_t last_pid, t_executor_ctx *ctx);

t_pipeline		*build_pipeline(t_list *tokens, t_executor_ctx *ctx);

//utils
int				ft_strcmp(const char *s1, const char *s2);
int				is_builtin(const char *cmd);
int				is_whitespace(char c);
int				is_operator_char(char c);
int				is_valid_var_char(char c);
int				is_valid_identifier(const char *str);
int				is_valid_integer(const char *str);
int				create_pipe(int pipe_fd[2], t_executor_ctx *ctx);
int				duplicate_fd(int fd, t_redirect_type type);
int				ft_isxdigit(int c);
long			hex_to_long(const char *hex);
char			*ft_strcat(char *dest, const char *src);
char			*ft_strncpy(char *dest, const char *src, size_t n);
char			*ft_strndup(const char *s1, size_t n);
void			ft_split_free(char **array);
void			close_fds_after_fork(int *prev_fd,
					int pipe_fd[2], int is_last);
char			*ft_strcpy(char *dst, const char *src);

void			save_stdio(int *in, int *out, int *err);
void			restore_stdio(int in, int out, int err);
void			add_export(t_executor_ctx *ctx,
					const char *name, bool assigned);
void			init_export_list_from_environ(t_executor_ctx *ctx);

t_export		*find_export(t_executor_ctx *ctx, const char *name);

void			remove_export(t_executor_ctx *ctx, const char *name);
void			sort_exports_insertion(t_export **arr, size_t n);
size_t			count_exports(t_export *head);

t_export		**list_to_array(t_export *head, size_t count);

void			print_exports(t_export **arr, size_t count);
void			print_environment(t_executor_ctx *ctx);
int				handle_setenv_and_export(t_executor_ctx *ctx,
					const char *name, const char *value);
int				process_export_args(char **argv, t_executor_ctx *ctx);
char			*parse_export_arg(const char *arg, char **name,
					char **error_part);
int				validate_export_identifier(const char *name,
					const char *error_part);
int				handle_single_export_arg(t_executor_ctx *ctx, const char *arg);
int				ret_cd_error(void);
int				ret_arg_error(void);
int				ret_no_arg(void);
void			restore_std_fds(int save_stdin,
					int save_stdout, int save_stderr);

char			*read_continued_input(char *full);

int set_env(const char *name, const char *value, int overwrite);


#endif
