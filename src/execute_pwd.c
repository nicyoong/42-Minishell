/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 21:58:06 by tching            #+#    #+#             */
/*   Updated: 2025/05/15 22:07:03 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_pwd(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int		save_stdout;
	char	cwd[PATH_MAX];

	(void)argv;
	save_stdout = dup(STDOUT_FILENO);
	if (setup_redirections(redirects, ctx) < 0)
	{
		dup2(save_stdout, STDOUT_FILENO);
		close(save_stdout);
		ctx->last_exit_status = 1;
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)))
	{
		printf("%s\n", cwd);
		ctx->last_exit_status = 0;
	}
	else
	{
		perror("pwd");
		ctx->last_exit_status = 1;
	}
	dup2(save_stdout, STDOUT_FILENO);
	close(save_stdout);
	return (ctx->last_exit_status);
}
