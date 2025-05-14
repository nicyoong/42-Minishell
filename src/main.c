/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 19:39:43 by tching            #+#    #+#             */
/*   Updated: 2025/05/15 01:31:26 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include "parser.h"
#include "executor.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "../libft/libft.h"
#include "utils.h"
 
int main(void)
{
    char *line;
    t_list *tokens;
    t_pipeline *pipeline;
    t_executor_ctx ctx = {0};
    ctx.last_exit_status = 0;
    setup_signal_handlers();
	//rl_bind_key('\t', rl_insert); checked bash Tab is meant to show files in cwd

    while (1) {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        char prompt[PATH_MAX + 16];
        snprintf(prompt, sizeof(prompt), "%s$ ", cwd);
        line = readline(prompt);
        if (!line) {
            exit(ctx.last_exit_status);
        }
        if (*line) add_history(line);
        tokens = lex_input(line);
        if (!tokens) {
            free(line);
            ctx.last_exit_status = 0;
            continue;
        }
        pipeline = parse(tokens);
        if (!pipeline) {
            ft_lstclear(&tokens, free_token);
            free(line);
            ctx.last_exit_status = 2;
            continue;
        }
        execute_pipeline(pipeline, &ctx);
        ft_lstclear(&tokens, free_token);
        free_pipeline(pipeline);
        free(line);
    }
    rl_clear_history();
    return (ctx.last_exit_status);
}
