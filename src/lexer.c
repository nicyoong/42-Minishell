#include "parser.h"

// ==============================
// Helper Functions
// ==============================
int is_whitespace(char c) { return (c == ' ' || c == '\t'); }

int is_operator_char(char c) { return (c == '|' || c == '<' || c == '>'); }

int is_valid_var_char(char c) { return (isalnum(c) || c == '_'); }

t_token *create_token(t_token_type type)
{
    t_token *token = malloc(sizeof(t_token));
    token->type = type;
    token->word = NULL;
    if (type == TOKEN_WORD) {
        token->word = malloc(sizeof(t_word));
        token->word->segments = NULL;
    }
    return token;
}

void add_segment(t_word *word, t_segment_type seg_type, const char *value)
{
    t_segment *seg = malloc(sizeof(t_segment));
    seg->type = seg_type;
    seg->value = ft_strdup(value);
    ft_lstadd_back(&word->segments, ft_lstnew(seg));
}

void free_token(void *token_ptr)
{
    t_token *token = (t_token *)token_ptr;
    if (token->word) {
        ft_lstclear(&token->word->segments, (void (*)(void *))free);
        free(token->word);
    }
    free(token);
}

// ==============================
// Lexer Logic
// ==============================

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
    } else {
        char var[1024];
        int var_idx = 0;
        while (is_valid_var_char(input[*i])) {
            var[var_idx++] = input[(*i)++];
        }
        var[var_idx] = '\0';
        add_segment(word, VARIABLE, var);
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

void process_unquoted_word(const char *input, int *i, t_word *word)
{
    char buffer[1024];
    int buf_idx = 0;

    while (input[*i] && !is_whitespace(input[*i]) && !is_operator_char(input[*i])) {
        if (input[*i] == '$') {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                add_segment(word, LITERAL, buffer);
                buf_idx = 0;
            }
            (*i)++;
            if (input[*i] == '?') {
                add_segment(word, EXIT_STATUS, "$?");
                (*i)++;
            } else {
                char var[1024];
                int var_idx = 0;
                while (is_valid_var_char(input[*i])) {
                    var[var_idx++] = input[(*i)++];
                }
                var[var_idx] = '\0';
                add_segment(word, VARIABLE, var);
            }
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

// // ==============================
// // Example Usage & Demonstration
// // ==============================
// int main() {
//     const char *example_cmd = "echo \"Hello $USER\" 'Single Quote $VAR' | cat << EOF > output.txt";
//     printf("Parsing command:\n%s\n\n", example_cmd);

//     t_list *tokens = lex_input(example_cmd);
//     if (!tokens) {
//         printf("Lexer error (e.g., unclosed quotes)\n");
//         return 1;
//     }

//     // Print tokens
//     int cmd_idx = 0;
//     for (t_list *curr = tokens; curr; curr = curr->next) {
//         t_token *token = (t_token *)curr->content;
//         printf("Token %d: ", ++cmd_idx);
        
//         if (token->type == TOKEN_WORD) {
//             printf("WORD [");
//             for (t_list *seg = token->word->segments; seg; seg = seg->next) {
//                 t_segment *s = (t_segment *)seg->content;
//                 printf("%s: '%s'", 
//                     (s->type == LITERAL) ? "LITERAL" :
//                     (s->type == VARIABLE) ? "VARIABLE" : "EXIT_STATUS",
//                     s->value);
//                 if (seg->next) printf(", ");
//             }
//             printf("]\n");
//         } else {
//             const char *type_str[] = {
//                 "PIPE", "REDIR_IN", "REDIR_OUT", "REDIR_APPEND", "REDIR_HEREDOC"
//             };
//             printf("%s\n", type_str[token->type - 1]);
//         }
//     }

//     ft_lstclear(&tokens, free_token);
//     return 0;
// }