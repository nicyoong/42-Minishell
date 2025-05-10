#ifndef UTILS_H
# define UTILS_H

# include <stdlib.h>
# include "../libft/libft.h"

int ft_strcmp(const char *s1, const char *s2);
void ft_split_free(char **array);
int  is_builtin(const char *cmd);
int is_whitespace(char c);
int is_operator_char(char c);
int is_valid_var_char(char c);
int is_valid_identifier(const char *str);
int is_valid_integer(const char *str);
char *ft_strjoin3(const char *s1, const char *s2, const char *s3);

# endif
