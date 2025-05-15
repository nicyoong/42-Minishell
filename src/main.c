/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 19:39:43 by tching            #+#    #+#             */
/*   Updated: 2025/05/16 00:12:46 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
 
// int main(void)
// {
//     char *line;
//     t_list *tokens;
//     t_pipeline *pipeline;
//     t_executor_ctx ctx = {0};
//     ctx.last_exit_status = 0;
//     setup_signal_handlers();
//     while (1) {
//         char cwd[PATH_MAX];
//         getcwd(cwd, sizeof(cwd));
//         char prompt[PATH_MAX + 16];
//         snprintf(prompt, sizeof(prompt), "%s$ ", cwd);
//         line = readline(prompt);
//         if (!line) {
//             exit(ctx.last_exit_status);
//         }
//         if (*line) add_history(line);
//         tokens = lex_input(line);
//         if (!tokens) {
//             free(line);
//             ctx.last_exit_status = 0;
//             continue;
//         }
//         pipeline = parse(tokens);
//         if (!pipeline) {
//             ft_lstclear(&tokens, free_token);
//             free(line);
//             ctx.last_exit_status = 2;
//             continue;
//         }
//         execute_pipeline(pipeline, &ctx);
//         ft_lstclear(&tokens, free_token);
//         free_pipeline(pipeline);
//         free(line);
//     }
//     rl_clear_history();
//     return (ctx.last_exit_status);
// }

char *read_line(t_executor_ctx *ctx)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    char prompt[PATH_MAX + 4];
    snprintf(prompt, sizeof(prompt), "%s$ ", cwd);

    char *line = readline(prompt);
    if (line && *line)
        add_history(line);
    return line;
}

t_list *tokenize_input(const char *line, t_executor_ctx *ctx)
{
    t_list *tokens = lex_input(line);
    if (!tokens)
        ctx->last_exit_status = 0;
    return tokens;
}
