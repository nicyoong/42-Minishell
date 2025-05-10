#include "utils.h"

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return ((unsigned char)*s1 - (unsigned char)*s2);
}

void ft_split_free(char **arr)
{
    if (!arr)
        return;
    for (int i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}

int is_builtin(const char *cmd) {
    return (cmd && (ft_strcmp(cmd, "cd") == 0 || 
                   ft_strcmp(cmd, "export") == 0 || 
                   ft_strcmp(cmd, "unset") == 0 ||
                   ft_strcmp(cmd, "pwd") == 0));
}
