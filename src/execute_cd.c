/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:44:19 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/24 18:13:51 by tiara            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	save_standard_fds(int *stdin_fd, int *stdout_fd, int *stderr_fd)
{
	*stdin_fd = dup(STDIN_FILENO);
	*stdout_fd = dup(STDOUT_FILENO);
	*stderr_fd = dup(STDERR_FILENO);
	return (*stdin_fd == -1 || *stdout_fd == -1 || *stderr_fd == -1);
}

static int	perform_cd_operation(char **argv, char **oldpwd)
{
	int	ret;

	*oldpwd = getcwd(NULL, 0);
	if (!*oldpwd)
		return (ret = ret_cd_error());
	if (!argv[1])
		ret = ret_no_arg();
	else if (argv[2])
		ret = ret_arg_error();
	else if (chdir(argv[1]) != 0)
		ret = ret_cd_error();
	else
		ret = 0;
	return (ret);
}

static int	update_pwd_vars(char *oldpwd)
{
	char	cwd[PATH_MAX];
	int		ret;

	ret = 0;
	setenv("OLDPWD", oldpwd, 1);
	if (!getcwd(cwd, sizeof(cwd)))
		return (ret = ret_cd_error());
	setenv("PWD", cwd, 1);
	return (ret);
}

int	handle_cd(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int		save_fds[3];
	int		ret;
	char	*oldpwd;

	if (save_standard_fds(&save_fds[0], &save_fds[1], &save_fds[2]))
		return (1);
	if (setup_redirections(redirects, ctx) < 0)
	{
		restore_std_fds(save_fds[0], save_fds[1], save_fds[2]);
		ctx->last_exit_status = 1;
		return (1);
	}
	ret = perform_cd_operation(argv, &oldpwd);
	if (ret == 0 && update_pwd_vars(oldpwd))
		ret = 1;
	free(oldpwd);
	restore_std_fds(save_fds[0], save_fds[1], save_fds[2]);
	ctx->last_exit_status = ret;
	return (ret);
}
