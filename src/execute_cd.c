/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:44:19 by tching            #+#    #+#             */
/*   Updated: 2025/05/16 13:45:39 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void restore_std_fds(int save_stdin, int save_stdout, int save_stderr)
{
    dup2(save_stdin, STDIN_FILENO);
    dup2(save_stdout, STDOUT_FILENO);
    dup2(save_stderr, STDERR_FILENO);
    close(save_stdin);
    close(save_stdout);
    close(save_stderr);
}

int handle_cd(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    int save_stderr = dup(STDERR_FILENO);
    int ret = 0;

    if (setup_redirections(redirects, ctx) < 0)
    {
        restore_std_fds(save_stdin, save_stdout, save_stderr);
        ctx->last_exit_status = 1;
        return 1;
    }
    if (!argv[1]) {
        char *home = getenv("HOME");
        if (!home) 
        {
            write(2,"cd: HOME not set\n", 17);
            ret = 1;
        } else {
            if (chdir(home) != 0) {
                perror("cd");
                ret = 1;
            }
        }
    } else if (argv[2]) {
        fprintf(stderr, "cd: too many arguments\n");
        ret = 1;
    } else {
        if (chdir(argv[1]) != 0) {
            perror("cd");
            ret = 1;
        }
    }
    restore_std_fds(save_stdin, save_stdout, save_stderr);
    ctx->last_exit_status = ret;
    return ret;
}

