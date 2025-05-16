/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:24:23 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/16 22:59:21 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char **environ;

void save_stdio(int *in, int *out, int *err)
{
	*in  = dup(STDIN_FILENO);
	*out = dup(STDOUT_FILENO);
	*err = dup(STDERR_FILENO);
}

void restore_stdio(int in, int out, int err)
{
	dup2(in,  STDIN_FILENO);
	dup2(out, STDOUT_FILENO);
	dup2(err, STDERR_FILENO);
	close(in);
	close(out);
	close(err);
}

int env_cmp(const void *a, const void *b)
{
    const char *va = *(const char * const *)a;
    const char *vb = *(const char * const *)b;
    /* compare up to the '=' */
    size_t la = strcspn(va, "=");
    size_t lb = strcspn(vb, "=");
    int cmp = strncmp(va, vb, la < lb ? la : lb);
    if (cmp != 0) return cmp;
    /* if one name is a prefix of the other, shorter one sorts first */
    return (int)(la - lb);
}

void print_environment(void)
{
    /* 1) count entries */
    size_t count = 0;
    for (char **e = environ; *e; e++)
        count++;

    /* 2) copy pointers into a temporary array */
    char **vars = malloc((count + 1) * sizeof(*vars));
    if (!vars) return; /* out of memory—just bail */
    for (size_t i = 0; i < count; i++)
        vars[i] = environ[i];
    vars[count] = NULL;

    /* 3) sort them by NAME */
    qsort(vars, count, sizeof(*vars), env_cmp);

    /* 4) print in bash-style, skipping LINES/COLUMNS */
    for (size_t i = 0; i < count; i++) {
        char *eq = strchr(vars[i], '=');
        if (!eq) continue;
        size_t namelen = eq - vars[i];
        if ((namelen == 5 && strncmp(vars[i], "LINES", 5) == 0) ||
            (namelen == 7 && strncmp(vars[i], "COLUMNS", 7) == 0) ||
			(namelen == 1 && vars[i][0] == '_'))
        {
            continue;
        }
        vars[i][namelen] = '\0';
        char *name  = vars[i];
        char *value = eq + 1;
        printf("declare -x %s=\"%s\"\n", name, value);
        vars[i][namelen] = '=';
    }

    free(vars);
}

int handle_single_export_arg(const char *arg)
{
    char *name = NULL;
    char *error_part = NULL;
    const char *eq = strchr(arg, '=');
    int ret = 0;

    if (eq) {
        name = ft_substr(arg, 0, eq - arg);
        error_part = ft_substr(arg, 0, eq - arg + 1);
    } else {
        name = ft_strdup(arg);
        error_part = ft_strdup(arg);
    }

    if (!is_valid_identifier(name)) {
        fprintf(stderr, "export: '%s': not a valid identifier\n", error_part);
        ret = 1;
    } else if (eq) {
        const char *value = eq + 1;
        if (setenv(name, value, 1) < 0) {
            perror("export");
            ret = 1;
        }
    } else {
        const char *current = getenv(name);
        if (setenv(name, current ? current : "", 1) < 0) {
            perror("export");
            ret = 1;
        }
    }
    free(name);
    free(error_part);
    return ret;
}

int process_export_args(char **argv)
{
    int ret = 0;
    for (int i = 1; argv[i]; i++) {
        if (handle_single_export_arg(argv[i]) != 0)
            ret = 1;
    }
    return ret;
}

int	execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_in, save_out, save_err;
    int ret = 0;

    save_stdio(&save_in, &save_out, &save_err);
    if (setup_redirections(redirects, ctx) < 0)
	{
        restore_stdio(save_in, save_out, save_err);
        ctx->last_exit_status = 1;
        return 1;
    }
    if (!argv[1])
        print_environment();
    else
        ret = process_export_args(argv);
    restore_stdio(save_in, save_out, save_err);
    ctx->last_exit_status = ret;
    return ret;
}

