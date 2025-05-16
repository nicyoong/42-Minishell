/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:24:23 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/16 22:45:19 by nyoong           ###   ########.fr       */
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

void print_environment(void)
{
    char **env = environ;
    while (*env) {
        printf("%s\n", *env);
        env++;
    }
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

// int execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
// {
// 	int save_stdin = dup(STDIN_FILENO);
// 	int save_stdout = dup(STDOUT_FILENO);
// 	int save_stderr = dup(STDERR_FILENO);
// 	int ret = 0;

// 	if (setup_redirections(redirects, ctx) < 0)
// 	{
// 		dup2(save_stdin, STDIN_FILENO);
// 		dup2(save_stdout, STDOUT_FILENO);
// 		dup2(save_stderr, STDERR_FILENO);
// 		close(save_stdin);
// 		close(save_stdout);
// 		close(save_stderr);
// 		ctx->last_exit_status = 1;
// 		return 1;
// 	}
// 	if (!argv[1])
// 	{
// 		extern char **environ;
// 		char **env = environ;
// 		while (*env) {
// 			printf("%s\n", *env);
// 			env++;
// 		}
// 	}
// 	else
// 	{
// 		int i = 1;
// 		while (argv[i]) {
// 			char *arg = argv[i];
// 			char *eq = strchr(arg, '=');
// 			char *name = NULL;
// 			char *error_part = NULL;
// 			int name_invalid = 0;

// 			if (eq) {
// 				name = ft_substr(arg, 0, eq - arg);
// 				error_part = ft_substr(arg, 0, eq - arg + 1);
// 			} else {
// 				name = ft_strdup(arg);
// 				error_part = ft_strdup(arg);
// 			}

// 			if (!is_valid_identifier(name)) {
// 				fprintf(stderr, "export: '%s': not a valid identifier\n", error_part);
// 				ret = 1;
// 				name_invalid = 1;
// 			}
// 			free(error_part);

// 			if (name_invalid) {
// 				free(name);
// 				i++;
// 				continue;
// 			}

// 			if (eq) {
// 				char *value = eq + 1;
// 				if (setenv(name, value, 1) != 0) {
// 					perror("export");
// 					ret = 1;
// 				}
// 			} else {
// 				char *current = getenv(name);
// 				if (setenv(name, current ? current : "", 1) != 0) {
// 					perror("export");
// 					ret = 1;
// 				}
// 			}

// 			free(name);
// 			i++;
// 		}
// 	}
// 	dup2(save_stdin, STDIN_FILENO);
// 	dup2(save_stdout, STDOUT_FILENO);
// 	dup2(save_stderr, STDERR_FILENO);
// 	close(save_stdin);
// 	close(save_stdout);
// 	close(save_stderr);

// 	ctx->last_exit_status = ret;
// 	return ret;
// }
