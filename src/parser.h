#ifndef PARSER_H
# define PARSER_H

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include "../libft/libft.h"

typedef enum e_token_type {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_REDIRECT_HEREDOC
} t_token_type;

typedef enum e_segment_type {
    LITERAL,
    VARIABLE,
    VAR_EXIT_STATUS
} t_segment_type;

typedef enum e_redirect_type {
    REDIR_IN,
    REDIR_OUT,
    REDIR_APPEND,
    REDIR_HEREDOC
} t_redirect_type;

typedef struct s_segment {
    t_segment_type type;
    char *value;
} t_segment;

typedef struct s_word {
    t_list *segments;
} t_word;

typedef struct s_token {
    t_token_type type;
    t_word *word;
} t_token;

typedef struct s_command {
    char **argv;
    t_list *redirects;  // List of t_redirect structures
} t_command;

typedef struct s_redirect {
    t_redirect_type type;
    char *filename;
} t_redirect;

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

// Main parsing functions
t_command *parse_command(t_list **tokens);
t_list *split_commands(t_list *tokens);
t_pipeline *parse(t_list *tokens);
void free_pipeline(t_pipeline *pipeline);

// Utility/printing functions
void print_word(t_word *word);
void print_pipeline(t_pipeline *pipeline);

# endif