/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 18:06:10 by tiara             #+#    #+#             */
/*   Updated: 2025/05/24 18:14:18 by tiara            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	restore_std_fds(int save_stdin, int save_stdout, int save_stderr)
{
	dup2(save_stdin, STDIN_FILENO);
	dup2(save_stdout, STDOUT_FILENO);
	dup2(save_stderr, STDERR_FILENO);
	close(save_stdin);
	close(save_stdout);
	close(save_stderr);
}

int	ret_cd_error(void)
{
	perror("cd");
	return (1);
}

int	ret_arg_error(void)
{
	write (2, "cd: too many arguments\n", 23);
	return (1);
}

int	ret_no_arg(void)
{
	char	*home;

	home = getenv("HOME");
	if (!home)
		write (2, "cd: HOME not set\n", 17);
	else
		if (chdir(home) != 0)
			perror("cd");
	return (1);
}
