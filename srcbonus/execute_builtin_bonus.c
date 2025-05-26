/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:34:39 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 03:00:44 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

int	execute_builtin(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	if (ft_strcmp(argv[0], "cd") == 0)
		return (handle_cd(argv, redirects, ctx));
	else if (ft_strcmp(argv[0], "export") == 0)
		return (execute_export(argv, redirects, ctx));
	else if (ft_strcmp(argv[0], "unset") == 0)
		return (execute_unset(argv, redirects, ctx));
	else if (ft_strcmp(argv[0], "pwd") == 0)
		return (execute_pwd(argv, redirects, ctx));
	else if (ft_strcmp(argv[0], "exit") == 0)
	{
		execute_exit(argv, ctx);
		return (0);
	}
	else if (ft_strcmp(argv[0], "echo") == 0)
		return (execute_echo(argv, redirects, ctx));
	else if (ft_strcmp(argv[0], "env") == 0)
		return (execute_env(argv, redirects, ctx));
	return (-1);
}

void	handle_builtin_command(char **argv, t_command *cmd, t_executor_ctx *ctx)
{
	int	status;

	status = 0;
	if (is_builtin(argv[0]))
	{
		status = execute_builtin(argv, cmd->redirects, ctx);
		ft_split_free(argv);
		exit(status);
	}
}
