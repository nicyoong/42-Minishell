/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 19:39:43 by tching            #+#    #+#             */
/*   Updated: 2025/03/19 20:27:10 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

/*
int	main(int argc, char **argv)
{
	//data structs for initialization

	//init data struct

	int	init_flag; //check if initialisation error
	//{ Check function }

	// init_flag = launch shell

	
	//free where needed
}
*/

int main() //general idea to start
{
	char	cmd[PATH_MAX];
	char	cwd[PATH_MAX];
	int	output;

	output = 0;
	if (getcwd(cwd, sizeof(cwd)))
		printf("%s", cwd); //print current working directory
	else
	{
		perror("getcwd");
	 	exit (EXIT_FAILURE);
	}


	while (1) //prompt for commands
	{
		printf("$ ");
		if (!fgets(cmd, PATH_MAX, stdin))
			break ;

		cmd[strcspn(cmd, "\n")] = '\0';

		if (strcmp(cmd, "exit") == 0)
			break;
	
		output = system(cmd);
		if (output == 1)
			perror("system");	
	}
	return (0);
}
