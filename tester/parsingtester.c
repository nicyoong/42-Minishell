#include "inc/minishell.h"
#include <stdio.h>
#include <string.h>

// Helper to print token types
const char *token_type_str(t_token_type type) {
    static const char *names[] = {
        "WORD", "PIPE", "REDIR_IN", "REDIR_OUT",
        "REDIR_APPEND", "REDIR_HEREDOC"
    };
    return names[type];
}

// Helper to print redirect types
const char *redirect_type_str(t_redirect_type type) {
    static const char *names[] = {
        "REDIR_IN", "REDIR_OUT", "REDIR_APPEND", "REDIR_HEREDOC"
    };
    return names[type];
}

// ==============================
// Lexer Test Cases
// ==============================
void test_lexer(const char *input, t_token *expected[], int count) {
    printf("\n=== Testing Lexer: '%s' ===\n", input);
    
    t_list *tokens = lex_input(input);
    if (!tokens) {
        printf("LEXER ERROR (returned NULL)\n");
        return;
    }

    t_list *current = tokens;
    int i = 0;
    while (current && i < count) {
        t_token *actual = current->content;
        t_token *exp = expected[i];

        // Check token type
        if (actual->type != exp->type) {
            printf("ERROR at token %d: Expected %s, got %s\n",
                   i+1, token_type_str(exp->type), token_type_str(actual->type));
            ft_lstclear(&tokens, free_token);
            return;
        }

        // For WORD tokens, check segments
        if (actual->type == TOKEN_WORD) {
            t_list *a_seg = actual->word->segments;
            t_list *e_seg = exp->word->segments;
            int seg_idx = 0;
            
            while (a_seg && e_seg) {
                t_segment *a = a_seg->content;
                t_segment *e = e_seg->content;

                if (a->type != e->type || strcmp(a->value, e->value) != 0) {
                    printf("ERROR at token %d segment %d: Expected %s('%s'), got %s('%s')\n",
                           i+1, seg_idx+1,
                           (e->type == LITERAL) ? "LITERAL" : "VARIABLE", e->value,
                           (a->type == LITERAL) ? "LITERAL" : "VARIABLE", a->value);
                    ft_lstclear(&tokens, free_token);
                    return;
                }
                a_seg = a_seg->next;
                e_seg = e_seg->next;
                seg_idx++;
            }

            if (a_seg || e_seg) {
                printf("ERROR at token %d: Segment count mismatch\n", i+1);
                ft_lstclear(&tokens, free_token);
                return;
            }
        }

        current = current->next;
        i++;
    }

    if (current || i < count) {
        printf("ERROR: Token count mismatch (expected %d, got %d)\n", count, i);
        ft_lstclear(&tokens, free_token);
        return;
    }

    printf("LEXER TEST PASSED\n");
    ft_lstclear(&tokens, free_token);
}

// ==============================
// Parser Test Cases
// ==============================
void test_parser(const char *input, int expected_commands, int expected_errors) {
    printf("\n=== Testing Parser: '%s' ===\n", input);
    
    t_list *tokens = lex_input(input);
    if (!tokens) {
        if (expected_errors) printf("PARSER TEST PASSED (expected error)\n");
        else printf("PARSER TEST FAILED (unexpected lexer error)\n");
        return;
    }

    t_pipeline *ast = parse(tokens);
    int has_error = (ast == NULL);

    if (has_error != expected_errors) {
        printf("PARSER TEST %s: Expected %s, got %s\n",
               expected_errors ? "MISSED ERROR" : "FALSE ERROR",
               expected_errors ? "error" : "success",
               has_error ? "error" : "success");
        ft_lstclear(&tokens, free_token);
        return;
    }

    if (!has_error) {
        int cmd_count = ft_lstsize(ast->commands);
        if (cmd_count != expected_commands) {
            printf("PARSER TEST FAILED: Expected %d commands, got %d\n",
                   expected_commands, cmd_count);
        } else {
            printf("PARSER TEST PASSED (%d commands)\n", cmd_count);
        }
    }

    free_pipeline(ast);
    ft_lstclear(&tokens, free_token);
}

// ==============================
// Test Scenarios
// ==============================
int main() {
    // Lexer test case 1: Basic command with variables
    t_token *tc1[] = {
        &(t_token){TOKEN_WORD, &(t_word){ft_lstnew(&(t_segment){LITERAL, "echo"})}},
        &(t_token){TOKEN_WORD, &(t_word){
            ft_lstnew_multi(2,
                &(t_segment){LITERAL, "Hello "},
                &(t_segment){VARIABLE, "USER"}
            )}},
        &(t_token){TOKEN_REDIRECT_OUT, NULL},
        &(t_token){TOKEN_WORD, &(t_word){ft_lstnew(&(t_segment){LITERAL, "file.txt"})}}
    };
    test_lexer("echo \"Hello $USER\" > file.txt", tc1, 4);

    // Lexer test case 2: Heredoc and pipes
    t_token *tc2[] = {
        &(t_token){TOKEN_WORD, &(t_word){ft_lstnew(&(t_segment){LITERAL, "cat"})}},
        &(t_token){TOKEN_REDIRECT_HEREDOC, NULL},
        &(t_token){TOKEN_WORD, &(t_word){ft_lstnew(&(t_segment){LITERAL, "EOF"})}},
        &(t_token){TOKEN_PIPE, NULL},
        &(t_token){TOKEN_WORD, &(t_word){ft_lstnew(&(t_segment){LITERAL, "wc"})}},
        &(t_token){TOKEN_WORD, &(t_word){ft_lstnew(&(t_segment){LITERAL, "-l"})}}
    };
    test_lexer("cat << EOF | wc -l", tc2, 6);

    // Parser test case 1: Valid pipeline
    test_parser("echo hello > file.txt | cat < input.txt", 2, 0);

    // Parser test case 2: Missing redirect filename
    test_parser("echo >", 0, 1);

    // Parser test case 3: Multiple redirects
    test_parser("cmd < in.txt >> out.txt 2> err.txt", 1, 0);

    // Parser test case 4: Empty input
    test_parser("", 0, 1);

    // Parser test case 5: Only redirects
    test_parser("<< EOF > file", 1, 0);

    return 0;
}