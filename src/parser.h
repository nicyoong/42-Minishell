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

# endif