#include "minishell_bonus.h"

char *get_word_str(const t_token *tok, const t_executor_ctx *ctx)
{
    size_t len = 0;
    t_list *seg;
    char   *s;

    // 1) compute length, accounting for VARIABLE/EXIT_STATUS
    for (seg = tok->word->segments; seg; seg = seg->next)
    {
        t_segment *se = seg->content;
        if (se->type == LITERAL || se->type == QUOTED_LITERAL)
            len += strlen(se->value);
        else if (se->type == VARIABLE)
        {
            char *v = getenv(se->value);
            if (v) len += strlen(v);
        }
        else if (se->type == EXIT_STATUS)
        {
            // worst‐case 3 digits + NUL
            len += 4;
        }
    }

    // 2) allocate
    s = malloc(len + 1);
    if (!s) return NULL;
    s[0] = '\0';

    // 3) fill
    for (seg = tok->word->segments; seg; seg = seg->next)
    {
        t_segment *se = seg->content;
        if (se->type == LITERAL || se->type == QUOTED_LITERAL)
        {
            strcat(s, se->value);
        }
        else if (se->type == VARIABLE)
        {
            char *v = getenv(se->value);
            if (v) strcat(s, v);
        }
        else if (se->type == EXIT_STATUS)
        {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", ctx->last_exit_status);
            strcat(s, buf);
        }
    }

    return s;
}

void free_match_node(void *content)
{
    (void)content;  // do nothing to the token pointer
}

int ft_strcasecmp(const char *s1, const char *s2)
{
    unsigned char c1, c2;
    while (*s1 || *s2)
    {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
        if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
        if (c1 != c2)
            return (int)c1 - (int)c2;
    }
    return 0;
}

int token_name_cmp(const t_list *a, const t_list *b)
{
    t_token *ta = a->content;
    t_token *tb = b->content;
    // we assume exactly one segment per word after wildcard expansion,
    // and that segment is plain text with the filename
    char *na = ((t_segment *)ta->word->segments->content)->value;
    char *nb = ((t_segment *)tb->word->segments->content)->value;
    return ft_strcasecmp(na, nb);
}
