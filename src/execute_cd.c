/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:44:19 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/12 20:22:14 by nyoong           ###   ########.fr       */
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

static int	update_pwd_vars(t_executor_ctx *ctx, char *oldpwd)
{
	char	cwd[PATH_MAX];
	int		ret;

	ret = 0;
	add_export(ctx, "OLDPWD", oldpwd, true);
	if (!getcwd(cwd, sizeof(cwd)))
		return (ret = ret_cd_error());
	add_export(ctx, "PWD", cwd, true);
	return (ret);
}

int	handle_heredoc_abort(int *save_fds, t_executor_ctx *ctx)
{
	restore_std_fds(save_fds[0], save_fds[1], save_fds[2]);
	return (ctx->last_exit_status);
}

int	handle_cd(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int				save_fds[3];
	int				ret;
	int				heredoc_fd;
	char			*oldpwd;
	t_redir_status	st;

	if (save_standard_fds(&save_fds[0], &save_fds[1], &save_fds[2]))
		return (1);
	st = setup_redirections(redirects, ctx, &heredoc_fd);
	if (st == HEREDOC_ABORT)
		return (handle_heredoc_abort(save_fds, ctx));
	if (st == REDIR_ERROR)
	{
		restore_std_fds(save_fds[0], save_fds[1], save_fds[2]);
		ctx->last_exit_status = 1;
		return (1);
	}
	ret = perform_cd_operation(argv, &oldpwd);
	if (ret == 0 && update_pwd_vars(ctx, oldpwd))
		ret = 1;
	free(oldpwd);
	restore_std_fds(save_fds[0], save_fds[1], save_fds[2]);
	ctx->last_exit_status = ret;
	return (ret);
}
