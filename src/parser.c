#include "parser.h"

// Helper: Check if token is a redirect
int is_redirect(t_token_type type)
{
    return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
            type == TOKEN_REDIRECT_APPEND || type == TOKEN_REDIRECT_HEREDOC);
}

// Helper: Convert token type to redirect type
t_redirect_type token_to_redirect(t_token_type type)
{
    if (type == TOKEN_REDIRECT_IN) return REDIR_IN;
    if (type == TOKEN_REDIRECT_OUT) return REDIR_OUT;
    if (type == TOKEN_REDIRECT_APPEND) return REDIR_APPEND;
    return REDIR_HEREDOC;
}

// Deep copy a word (for AST ownership)
t_word *copy_word(t_word *src)
{
    t_word *dst = ft_calloc(1, sizeof(t_word));
    t_list *segments = NULL;
    
    for (t_list *tmp = src->segments; tmp; tmp = tmp->next) {
        t_segment *src_seg = tmp->content;
        t_segment *dst_seg = ft_calloc(1, sizeof(t_segment));
        dst_seg->type = src_seg->type;
        dst_seg->value = ft_strdup(src_seg->value);
        ft_lstadd_back(&segments, ft_lstnew(dst_seg));
    }
    dst->segments = segments;
    return dst;
}

// Free a word and its segments
void free_word(void *word_ptr)
{
    t_word *w = word_ptr;
    ft_lstclear(&w->segments, (void (*)(void *))free);
    free(w);
}

// Free a redirect and its filename
void free_redirect(void *redir_ptr)
{
    t_redirect *r = redir_ptr;
    free_word(r->filename);
    free(r);
}

// Free entire command structure
void free_command(void *cmd_ptr)
{
    t_command *cmd = cmd_ptr;
    ft_lstclear(&cmd->arguments, free_word);
    ft_lstclear(&cmd->redirects, free_redirect);
    free(cmd);
}

int handle_redirect(t_list **tokens, t_command *cmd)
{
    t_token *token = (t_token *)(*tokens)->content;
    t_redirect *redir = ft_calloc(1, sizeof(t_redirect));

    redir->type = token_to_redirect(token->type);
    *tokens = (*tokens)->next;

    if (!*tokens || ((t_token *)(*tokens)->content)->type != TOKEN_WORD)
    {
        free(redir);
        return 0;
    }
    t_word *filename = copy_word(((t_token *)(*tokens)->content)->word);
    redir->filename = filename;
    ft_lstadd_back(&cmd->redirects, ft_lstnew(redir));
    *tokens = (*tokens)->next;
    return 1;
}

t_command *parse_command(t_list **tokens)
{
    t_command *cmd = ft_calloc(1, sizeof(t_command));
    
    while (*tokens) {
        t_token *token = (*tokens)->content;
        
        if (is_redirect(token->type)) {
            if (!handle_redirect(tokens, cmd)) {
                free_command(cmd);
                return NULL;
            }
            continue;
        } 
        else if (token->type == TOKEN_WORD) {
            t_word *arg = copy_word(token->word);
            ft_lstadd_back(&cmd->arguments, ft_lstnew(arg));
            *tokens = (*tokens)->next;
        } 
        else {
            free_command(cmd);
            return NULL;
        }
    }
    return cmd;
}

t_list *split_commands(t_list *tokens)
{
    t_list *cmds = NULL;
    t_list *current = NULL;
    
    while (tokens) {
        t_token *token = tokens->content;
        
        if (token->type == TOKEN_PIPE)
        {
            if (!current)
            {
                ft_lstclear(&cmds, free);
                return NULL;
            }
            ft_lstadd_back(&cmds, ft_lstnew(current));
            current = NULL;
        } 
        else {
            t_list *new_node = ft_lstnew(token);
            if (!current) {
                current = new_node;
            } else {
                ft_lstadd_back(&current, new_node);
            }
        }
        tokens = tokens->next;
    }
    if (current) 
        ft_lstadd_back(&cmds, ft_lstnew(current));
    return cmds;
}

// Main parser function
t_pipeline *parse(t_list *tokens)
{
    t_pipeline *pipeline = ft_calloc(1, sizeof(t_pipeline));
    t_list *cmd_groups = split_commands(tokens);
    
    if (!cmd_groups)
    {
        free(pipeline);
        return NULL;
    }
    t_list *curr_group = cmd_groups;
    while (curr_group)
    {
        t_list **cmd_tokens = (t_list **)&curr_group->content;
        t_command *cmd = parse_command(cmd_tokens);
        
        if (!cmd || *cmd_tokens != NULL) {
            ft_lstclear(&pipeline->commands, free_command);
            free(pipeline);
            t_list *temp = cmd_groups;
            while (temp) {
                t_list *tokens = temp->content;
                ft_lstclear(&tokens, NULL);
                temp = temp->next;
            }
            ft_lstclear(&cmd_groups, NULL);
            return NULL;
        }
        ft_lstadd_back(&pipeline->commands, ft_lstnew(cmd));
        curr_group = curr_group->next;
    }
    t_list *temp = cmd_groups;
    while (temp)
    {
        t_list *tokens = temp->content;
        ft_lstclear(&tokens, NULL);
        temp = temp->next;
    }
    ft_lstclear(&cmd_groups, NULL);
    return pipeline;
}

const char *redirect_type_str(t_redirect_type type)
{
	const char *names[] = {
		[REDIR_IN] = "INPUT",
		[REDIR_OUT] = "OUTPUT",
		[REDIR_APPEND] = "APPEND",
		[REDIR_HEREDOC] = "HEREDOC"
	};
	return names[type];
}

void free_pipeline(t_pipeline *pipeline)
{
    ft_lstclear(&pipeline->commands, free_command);
    free(pipeline);
}

void print_word(t_word *word)
{
    char buffer[1024] = {0};
    for (t_list *seg = word->segments; seg; seg = seg->next) {
        t_segment *s = seg->content;
        if (s->type == VARIABLE) {
            strcat(buffer, "$");
        } else if (s->type == EXIT_STATUS) {
            strcat(buffer, "$?");
        }
        strcat(buffer, s->value);
    }
    printf("%s", buffer);
}

void print_pipeline(t_pipeline *pipeline)
{
    int cmd_idx = 0;
    for (t_list *cmd_node = pipeline->commands; cmd_node; cmd_node = cmd_node->next) {
        t_command *cmd = cmd_node->content;
        printf("\nCommand %d:\n", ++cmd_idx);
        printf("  Arguments: [");
        for (t_list *arg_node = cmd->arguments; arg_node; arg_node = arg_node->next) {
            t_word *word = arg_node->content;
            printf("\"");
            print_word(word);
            printf("\"%s", arg_node->next ? ", " : "");
        }
        printf("]\n");
        if (cmd->redirects) {
            printf("  Redirects:\n");
            for (t_list *redir_node = cmd->redirects; redir_node; redir_node = redir_node->next) {
                t_redirect *r = redir_node->content;
                printf("    %s -> \"", redirect_type_str(r->type));
                print_word(r->filename);
                printf("\"\n");
            }
        }
    }
    printf("\n");
}

// int main() {
//     const char *test_cmd = "echo \"Hello $USER\" 'Single Quote $VAR' | cat << EOF > output.txt";
    
//     printf("Testing parser with command:\n%s\n\n", test_cmd);

//     // Lexing stage
//     t_list *tokens = lex_input(test_cmd);
//     if (!tokens) {
//         printf("Lexer error\n");
//         return 1;
//     }

//     // Print tokens from lexer
//     printf("Lexer output:\n");
//     int idx = 0;
//     for (t_list *curr = tokens; curr; curr = curr->next) {
//         t_token *t = curr->content;
//         printf("Token %2d: ", ++idx);
//         if (t->type == TOKEN_WORD) {
//             printf("WORD [");
//             print_word(t->word);
//             printf("]\n");
//         } else {
//             const char *types[] = {"PIPE", "REDIR_IN", "REDIR_OUT", 
//                                  "REDIR_APPEND", "REDIR_HEREDOC"};
//             printf("%s\n", types[t->type - 1]);
//         }
//     }

//     // Parsing stage
//     t_pipeline *pipeline = parse(tokens);
//     if (!pipeline) {
//         printf("\nParser error!\n");
//         ft_lstclear(&tokens, free_token);
//         return 1;
//     }

//     // Print parsed structure
//     printf("\nParser output:");
//     print_pipeline(pipeline);

//     // Cleanup
//     ft_lstclear(&tokens, free_token);
//     free_pipeline(pipeline);
//     return 0;
// }
