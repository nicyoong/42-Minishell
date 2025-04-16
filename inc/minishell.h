/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 22:19:31 by tching            #+#    #+#             */
/*   Updated: 2025/03/18 22:46:06 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <error.h>
# include <stdbool.h>
# include <fcntl.h>
# include <limits.h>
# include <signal.h>
# include <termios.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <dirent.h>
// # include <readline/readline.h>
// # include <readline/history.h>
// # include <curses.h>

//DEFINES
# define MEM_FAIL	"Memory allocation failed."
# define PIPE_FAIL	"The pipe() failed."
# define FORK_FAIL	"The fork() failed."
# define CLOSE_FAIL	"The close() failed."
# define DUP_FAIL	"The dup() failed."
# define DUP2_FAIL	"The dup2() failed."
# define EXECVE_FAIL	"The execve() failed."

//LEXER

// ==============================
// Data Structures (from earlier)
// ==============================
typedef enum e_segment_type {
    LITERAL,
    VARIABLE,
    VAR_EXIT_STATUS
} t_segment_type;

typedef struct s_segment {
    t_segment_type type;
    char *value;
} t_segment;

typedef struct s_word {
    t_list *segments;  // List of t_segment*
} t_word;

typedef enum e_token_type {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_REDIRECT_HEREDOC,
} t_token_type;

typedef struct s_token {
    t_token_type type;
    t_word *word;  // NULL for non-WORD tokens
} t_token;

//PARSER

typedef enum e_redirect_type {
    REDIR_IN,
    REDIR_OUT,
    REDIR_APPEND,
    REDIR_HEREDOC
} t_redirect_type;

typedef struct s_redirect {
    t_redirect_type type;
    struct s_word   *filename;  // For HEREDOC, delimiter
} t_redirect;

typedef struct s_command {
    t_list          *arguments;  // List of t_word*
    t_list          *redirects;  // List of t_redirect*
} t_command;

typedef struct s_pipeline {
    t_list          *commands;   // List of t_command*
} t_pipeline;

// Parser functions
t_pipeline         *parse(t_list *tokens);
void               free_pipeline(t_pipeline *pipeline);

//TERMINAL





//SHELL


//EXPANSION


//EXECUTION


//SIGNALS


//ERROR HANDLING


//FREE


//EXIT





#endif
