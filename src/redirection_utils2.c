/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:33:54 by tching            #+#    #+#             */
/*   Updated: 2025/05/17 22:41:46 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	open_redir_input(const char *path)
{
	return (open(path, O_RDONLY));
}

int	open_redir_output(const char *path)
{
	int		flags;
	mode_t	mode;

	flags = O_WRONLY | O_CREAT | O_TRUNC;
	mode = 0644;
	return (open(path, flags, mode));
}

int	open_redir_append(const char *path)
{
	int		flags;
	mode_t	mode;

	flags = O_WRONLY | O_CREAT | O_APPEND;
	mode = 0644;
	return (open(path, flags, mode));
}

int	open_redirection_fd(t_redirect_type type,
		const char *path, t_word *filename, t_executor_ctx *ctx)
{
	if (type == REDIR_IN)
		return (open_redir_input(path));
	else if (type == REDIR_OUT)
		return (open_redir_output(path));
	else if (type == REDIR_APPEND)
		return (open_redir_append(path));
	else if (type == REDIR_HEREDOC)
		return (process_heredoc(filename, ctx));
	else
		return (-1);
}
