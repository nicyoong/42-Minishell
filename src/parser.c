#include "inc/minishell.h"

// Helper: Check if token is a redirect
static int is_redirect(t_token_type type) {
    return (type == TOKEN_REDIRECT_IN || type == TOKEN_REDIRECT_OUT ||
            type == TOKEN_REDIRECT_APPEND || type == TOKEN_REDIRECT_HEREDOC);
}

// Helper: Convert token type to redirect type
static t_redirect_type token_to_redirect(t_token_type type) {
    if (type == TOKEN_REDIRECT_IN) return REDIR_IN;
    if (type == TOKEN_REDIRECT_OUT) return REDIR_OUT;
    if (type == TOKEN_REDIRECT_APPEND) return REDIR_APPEND;
    return REDIR_HEREDOC;
}

// Deep copy a word (for AST ownership)
static t_word *copy_word(t_word *src) {
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
static void free_word(void *word_ptr) {
    t_word *w = word_ptr;
    ft_lstclear(&w->segments, (void (*)(void *))free);
    free(w);
}

// Free a redirect and its filename
static void free_redirect(void *redir_ptr) {
    t_redirect *r = redir_ptr;
    free_word(r->filename);
    free(r);
}

// Free entire command structure
static void free_command(void *cmd_ptr) {
    t_command *cmd = cmd_ptr;
    ft_lstclear(&cmd->arguments, free_word);
    ft_lstclear(&cmd->redirects, free_redirect);
    free(cmd);
}

// Parse tokens for a single command
static t_command *parse_command(t_list **tokens) {
    t_command *cmd = ft_calloc(1, sizeof(t_command));
    
    while (*tokens) {
        t_token *token = (*tokens)->content;
        
        if (is_redirect(token->type)) {
            // Handle redirect: expect next token as filename
            t_redirect *redir = ft_calloc(1, sizeof(t_redirect));
            redir->type = token_to_redirect(token->type);
            
            *tokens = (*tokens)->next;  // Move to filename token
            if (!*tokens || ((t_token*)(*tokens)->content)->type != TOKEN_WORD) {
                free_command(cmd); free(redir);
                return NULL;  // Syntax error: missing filename
            }
            
            // Copy filename word
            t_word *filename = copy_word(((t_token*)(*tokens)->content)->word);
            redir->filename = filename;
            ft_lstadd_back(&cmd->redirects, ft_lstnew(redir));
            
            *tokens = (*tokens)->next;  // Consume filename token
        } 
        else if (token->type == TOKEN_WORD) {
            // Add argument (copy the word)
            t_word *arg = copy_word(token->word);
            ft_lstadd_back(&cmd->arguments, ft_lstnew(arg));
            *tokens = (*tokens)->next;
        } 
        else {
            free_command(cmd);
            return NULL;  // Invalid token in command
        }
    }
    return cmd;
}

// Split tokens into commands by PIPE
static t_list *split_commands(t_list *tokens) {
    t_list *cmds = NULL;
    t_list *current = NULL;
    
    while (tokens) {
        t_token *token = tokens->content;
        
        if (token->type == TOKEN_PIPE) {
            if (!current) {  // Pipe at start
                ft_lstclear(&cmds, free_command);
                return NULL;
            }
            ft_lstadd_back(&cmds, current);
            current = NULL;
        } 
        else {
            if (!current) current = ft_lstnew(NULL);
            ft_lstadd_back(&current, ft_lstnew(token));
        }
        tokens = tokens->next;
    }
    if (current) ft_lstadd_back(&cmds, current);
    return cmds;
}

// Main parser function
t_pipeline *parse(t_list *tokens) {
    t_pipeline *pipeline = ft_calloc(1, sizeof(t_pipeline));
    t_list *cmd_groups = split_commands(tokens);
    
    if (!cmd_groups) {
        free(pipeline);
        return NULL;  // Syntax error in command splitting
    }
    
    for (t_list *group = cmd_groups; group; group = group->next) {
        t_list *cmd_tokens = group->content;
        t_command *cmd = parse_command(&cmd_tokens);
        
        if (!cmd || cmd_tokens != NULL) {  // cmd_tokens should be fully consumed
            ft_lstclear(&cmd_groups, (void (*)(void *))ft_lstclear);
            free_pipeline(pipeline);
            return NULL;
        }
        ft_lstadd_back(&pipeline->commands, ft_lstnew(cmd));
    }
    
    ft_lstclear(&cmd_groups, (void (*)(void *))ft_lstclear);
    return pipeline;
}

void free_pipeline(t_pipeline *pipeline) {
    ft_lstclear(&pipeline->commands, free_command);
    free(pipeline);
}

void print_word(t_word *word) {
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