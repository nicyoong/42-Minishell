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
