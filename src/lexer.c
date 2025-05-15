#include "minishell.h"

void handle_variable_expansion(const char *input, int *i, t_word *word, char *buffer, int *buf_idx)
{
    if (*buf_idx > 0)
    {
        buffer[*buf_idx] = '\0';
        add_segment(word, LITERAL, buffer);
        *buf_idx = 0;
    }
    (*i)++;
    if (input[*i] == '?')
    {
        add_segment(word, EXIT_STATUS, "$?");
        (*i)++;
    }
    else 
    {
        if (is_valid_var_char(input[*i])) 
        {
            char var[1024];
            int var_idx = 0;
            while (is_valid_var_char(input[*i])) 
            {
                var[var_idx++] = input[(*i)++];
            }
            var[var_idx] = '\0';
            add_segment(word, VARIABLE, var);
        } 
        else 
        {
            add_segment(word, LITERAL, "$");
        }
    }
}

int process_quoted_content(const char *input, int *i, char quote_type, t_word *word)
{
    char buffer[1024];
    int buf_idx = 0;

    (*i)++;
    while (input[*i] && input[*i] != quote_type) {
        if (quote_type == '"' && input[*i] == '$') {
            handle_variable_expansion(input, i, word, buffer, &buf_idx);
        } else {
            buffer[buf_idx++] = input[(*i)++];
        }
    }
    if (buf_idx > 0) {
        buffer[buf_idx] = '\0';
        add_segment(word, LITERAL, buffer);
    }
    if (input[*i] != quote_type) return 0;
    (*i)++;
    return 1;
}

void process_unquoted_segment(const char *input, int *i, t_word *word)
{
    char buffer[1024];
    int buf_idx = 0;

    while (input[*i] && !is_whitespace(input[*i]) && !is_operator_char(input[*i]) 
            && input[*i] != '\'' && input[*i] != '"') {
        if (input[*i] == '$') {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                add_segment(word, LITERAL, buffer);
                buf_idx = 0;
            }
            handle_variable_expansion(input, i, word, buffer, &buf_idx);
        } else {
            buffer[buf_idx++] = input[(*i)++];
        }
    }
    if (buf_idx > 0) {
        buffer[buf_idx] = '\0';
        add_segment(word, LITERAL, buffer);
    }
}

t_token_type get_operator(const char *input, int *i)
{
    if (input[*i] == '|') {
        (*i)++;
        return TOKEN_PIPE;
    } else if (input[*i] == '<') {
        if (input[*i + 1] == '<') {
            (*i) += 2;
            return TOKEN_REDIRECT_HEREDOC;
        }
        (*i)++;
        return TOKEN_REDIRECT_IN;
    } else if (input[*i] == '>') {
        if (input[*i + 1] == '>') {
            (*i) += 2;
            return TOKEN_REDIRECT_APPEND;
        }
        (*i)++;
        return TOKEN_REDIRECT_OUT;
    }
    return TOKEN_WORD;
}

t_list *lex_input(const char *input)
{
    t_list *tokens = NULL;
    int i = 0;
    int len = strlen(input);

    while (i < len) {
        while (i < len && is_whitespace(input[i])) i++;
        if (i >= len) break;

        if (is_operator_char(input[i])) {
            t_token_type type = get_operator(input, &i);
            ft_lstadd_back(&tokens, ft_lstnew(create_token(type)));
        } else {
            t_token *token = create_token(TOKEN_WORD);
            while (i < len && !is_whitespace(input[i]) && !is_operator_char(input[i])) {
                if (input[i] == '\'' || input[i] == '"') {
                    char quote_type = input[i];
                    if (!process_quoted_content(input, &i, quote_type, token->word)) {
                        ft_lstclear(&tokens, free_token);
                        free_token(token);
                        return NULL;
                    }
                } else {
                    process_unquoted_segment(input, &i, token->word);
                }
            }
            ft_lstadd_back(&tokens, ft_lstnew(token));
        }
    }
    return tokens;
}

