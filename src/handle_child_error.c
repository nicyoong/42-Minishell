/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_child_error.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 19:39:35 by tching            #+#    #+#             */
/*   Updated: 2025/05/17 19:51:18 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exit_with_error(char *msg, char *cmd, int code)
{
	write(STDERR_FILENO, "minishell: ", 11);
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, msg, ft_strlen(msg));
	write(STDERR_FILENO, "\n", 1);
	exit(code);
}

static void	print_enoent(char *cmd)
{
	if (ft_strchr(cmd, '/') != NULL)
		exit_with_error("No such file or directory", cmd, 127);
	else
		exit_with_error("command not found", cmd, 127);
}

void	handle_invalid_arguments(char **argv)
{
	if (!argv || !argv[0] || argv[0][0] == '\0')
	{
		ft_split_free(argv);
		exit(0);
	}
}

void	handle_path_errors(char *path, char **argv)
{
	if (path)
		return ;
	if (errno == EACCES)
		exit_with_error("Permission denied", argv[0], 126);
	else if (errno == EISDIR)
		exit_with_error("Is a directory", argv[0], 126);
	else if (errno == ENOENT)
	{
		print_enoent(argv[0]);
		ft_split_free(argv);
		exit(127);
	}
	else
		exit_with_error("command not found", argv[0], 127);
}
