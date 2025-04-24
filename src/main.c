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
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>

/*
int	main(int argc, char **argv)
{
	//data structs for initialization
gcc
	//init data struct

	int	init_flag; //check if initialisation error
	//{ Check function }

	// init_flag = launch shell

	
	//free where needed
}*/
/*
void	execute_command(char **args)
{
	pid_t	pid;
	int	status;

	pid = fork();
	if (pid == 0)
	{
		if (execvp(args[0], args))
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0)
		perror("fork");
	else
	{
		waitpid(pid, &status, WUNTRACED);
		while (!WIFEXITED(status) && !WIFSIGNALED(status))
			waitpid(pid, &status, WUNTRACED);
	}
}

bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

char	**parse_command(char *cmd)
{
	char	**args;
	int	i;
	int	j;
	int	k;
	int	start;
	bool	flag;
	
	
	i = 0;
	j = 0;
	k = 0;
	start = 0;
	flag = false;
	args = malloc(64 * sizeof(char *));
	if (!args)
		return (NULL);
	while (cmd[j])
	{
		if (!is_whitespace(cmd[j]))
		{
			if (!flag)
			{
				start = j;
				flag = true;
			}
		}
		else
		{
			if (flag)
			{
				args[i] = malloc(j - start + 1);
				if (!args[i])
				{
					if (k < i)
					{
						free (args[k]);
						k++;
					}
					free (args);
					return (NULL);
				}
				strncpy(args[i], cmd + start, j - start);
				args[i][j - start] = '\0';
				i++;
				flag = false;
			}
		}
		j++;
	}

	if (flag)
	{
		args[i] = malloc(strlen(cmd + start) + 1);
		if (!args[i])
		{
			k = 0;
			while (k < i)
			{
				free(args);
				k++;
			}
			free(args);
			return NULL;
		}
		strcpy(args[i], cmd + start);
		i++;
	}
	args[i] = NULL;
	return (args);
}	 

void	print_prompt()
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)))
		printf("\033[1;32m%s\033[0m$ ", cwd);
	else
	{
		perror("getcwd");
		exit (EXIT_FAILURE);
	}
}

int	main()
{
	char	*cmd;
	char	**args;

	while (1)
	{
		print_prompt();
		
		cmd  = readline ("");
		if (!cmd)
			break;
		if (*cmd)
			add_history(cmd);
		if (strcmp(cmd, "exit") == 0)
		{
			free (cmd);
			break ;
		}

		args = parse_command(cmd);
		if (args[0] != NULL)
			execute_command(args);
		free(args);
		free(cmd);
	}
	return (0);
}	*/

#include "parser.h"
#include "executor.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "../libft/libft.h"

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
        execute_pipeline(pipeline); //CORE EXECUTION
        ft_lstclear(&tokens, free_token);
        free_pipeline(pipeline);
        free(line);
    }
    return (0);
}


