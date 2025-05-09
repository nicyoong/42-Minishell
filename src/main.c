/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 19:39:43 by tching            #+#    #+#             */
/*   Updated: 2025/03/24 22:38:40 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include <signals.h>
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
/*
int main(void)
{
    char *line;
    t_list *tokens;
    t_pipeline *pipeline;

    while (1) 
	{
        line = readline("minishell$ ");
        if (!line)
            break;
        if (*line)
            add_history(line);
	if (ft_strcmp(line, "exit") == 0)
	{
		free (line);
		break;
	}
        tokens = lex_input(line);
        if (!tokens) 
		{
            free(line);
            continue;
        }
        pipeline = parse(tokens);
        if (!pipeline)
		{
            printf("Parser error (e.g., bad syntax)\n");
            ft_lstclear(&tokens, free_token);
            free(line);
            continue;
        }
        execute_pipeline(pipeline);
        ft_lstclear(&tokens, free_token);
        free_pipeline(pipeline);
        free(line);
    }
    return (0);
}*/

void sigint_handler(int signo)
{
    (void)signo;
    write(STDOUT_FILENO, "\n", 1);
    rl_replace_line("", 0);
    rl_on_new_line();
    rl_redisplay();
}

void setup_signal_handlers(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa_int, NULL) == -1)
        perror("sigaction SIGINT");

    sa_quit.sa_handler = SIG_IGN;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
        perror("sigaction SIGQUIT");
}

int main(void)
{
    char *line;
    t_list *tokens;
    t_pipeline *pipeline;
    t_executor_ctx ctx = {0};
    setup_signal_handlers();
	
    while (1)
	{
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        
        char prompt[PATH_MAX + 16];
        snprintf(prompt, sizeof(prompt), "%s$ ", cwd);

        line = readline(prompt);
        if (!line) break;
        if (*line) add_history(line);
        
        if (ft_strcmp(line, "exit") == 0)
		{
            free(line);
            break;
        }

        tokens = lex_input(line);
        if (!tokens)
		{
            free(line);
            continue;
        }

        pipeline = parse(tokens);
        if (!pipeline) {
            printf("Parser error\n");
            ft_lstclear(&tokens, free_token);
            free(line);
            continue;
        }

        execute_pipeline(pipeline, &ctx);
        
        ft_lstclear(&tokens, free_token);
        free_pipeline(pipeline);
        free(line);
    }
    return (ctx.last_exit_status);
}
