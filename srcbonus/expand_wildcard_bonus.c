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

void insert_token_sorted(t_list **head, t_list *node)
{
    if (!*head
     || token_name_cmp(node, *head) < 0)
    {
        // Insert at front
        node->next = *head;
        *head = node;
        return;
    }

    // Find insertion point
    t_list *prev = *head;
    while (prev->next
        && token_name_cmp(node, prev->next) >= 0)
    {
        prev = prev->next;
    }
    // Insert after prev
    node->next     = prev->next;
    prev->next     = node;
}

int wildcard_match(const char *pat, const char *str)
{
    if (*pat == '\0')
        return *str == '\0';
    if (*pat == '*') {
        // match '*' as empty (pat+1, str) or consume one char (pat, str+1)
        return wildcard_match(pat + 1, str)
            || (*str && wildcard_match(pat, str + 1));
    }
    if (*pat == *str)
        return wildcard_match(pat + 1, str + 1);
    return 0;
}

t_list *match_in_cwd(const char *pattern)
{
    DIR           *d      = opendir(".");
    struct dirent *entry;
    t_list        *matches = NULL;

    if (!d)
        return NULL;

    while ((entry = readdir(d)))
    {
        char *name = entry->d_name;

        // skip "." and ".."
        if (name[0] == '.' &&
           (name[1] == '\0' ||
           (name[1] == '.' && name[2] == '\0')))
            continue;

        // skip dot-files unless pattern starts with '.'
        if (name[0] == '.' && pattern[0] != '.')
            continue;

        if (wildcard_match(pattern, name))
        {
            // build the token
            t_token *tok = malloc(sizeof(*tok));
            tok->type  = TOKEN_WORD;
            tok->word  = malloc(sizeof(*tok->word));
            tok->word->segments = NULL;
            add_segment(tok->word, LITERAL, name);

            // wrap in a list node
            t_list *node = ft_lstnew(tok);

            // insert sorted
            insert_token_sorted(&matches, node);
        }
    }

    closedir(d);
    return matches;  // NULL if no matches
}

t_token *copy_token(const t_token *orig)
{
    t_token *dup = malloc(sizeof(*dup));
    dup->type = orig->type;

    if (orig->type == TOKEN_WORD && orig->word)
    {
        dup->word = malloc(sizeof(*dup->word));
        dup->word->segments = NULL;

        for (t_list *s = orig->word->segments; s; s = s->next)
        {
            t_segment *seg = s->content;
            t_segment_type use_type = seg->type;

            // if this was a quoted literal, treat it as plain LITERAL
            if (use_type == QUOTED_LITERAL)
                use_type = LITERAL;

            add_segment(dup->word, use_type, seg->value);
        }
    }
    else
        dup->word = NULL;
    return dup;
}


