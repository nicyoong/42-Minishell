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

int main(void)
{
    char *line;
    t_list *tokens;
    t_pipeline *pipeline;

    while (1) 
    {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        
        char prompt[PATH_MAX + 16]; // add space for shell prompt label
        snprintf(prompt, sizeof(prompt), "%s$ ", cwd); // will need to rebuild this, make mem safe.

        line = readline(prompt);
        if (!line)
            break;
        if (*line)
            add_history(line);
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
}
