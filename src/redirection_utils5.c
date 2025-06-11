/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils5.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 01:15:11 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/12 01:26:04 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_errno(char *prog, t_word *path_word)
{
	t_list		*seg_node;
	t_segment	*seg;

	ft_putstr_fd(prog, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	seg_node = path_word->segments;
	while (seg_node)
	{
		seg = (t_segment *)seg_node->content;
		ft_putstr_fd(seg->value, STDERR_FILENO);
		seg_node = seg_node->next;
	}
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(strerror(errno), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

t_redir_status	check_open_fd(int fd,
	t_word *filename, t_executor_ctx *ctx)
{
	if (fd < 0)
	{
		print_errno("minishell", filename);
		ctx->last_exit_status = 1;
		return (REDIR_ERROR);
	}
	return (REDIR_OK);
}

t_redir_status	check_duplicate_fd(int fd,
	t_redirect_type type, t_executor_ctx *ctx)
{
	if (duplicate_fd(fd, type) < 0)
	{
		ctx->last_exit_status = 1;
		close(fd);
		return (REDIR_ERROR);
	}
	return (REDIR_OK);
}

t_redir_status	handle_regular_redirect(t_redirect *r, t_executor_ctx *ctx)
{
	char	path[4096];
	char	*trimmed;
	int		fd;

	if (build_path_from_word(r->filename, path, sizeof(path), ctx) < 0)
	{
		ctx->last_exit_status = 1;
		return (REDIR_ERROR);
	}
	trimmed = trim_and_validate_path(path);
	if (!trimmed)
	{
		ctx->last_exit_status = 1;
		return (REDIR_ERROR);
	}
	ft_strncpy(path, trimmed, sizeof(path) - 1);
	path[sizeof(path) - 1] = '\0';
	free(trimmed);
	fd = open_redirection_fd(r->type, path, r->filename, ctx);
	if (check_open_fd(fd, r->filename, ctx) == REDIR_ERROR)
		return (REDIR_ERROR);
	if (check_duplicate_fd(fd, r->type, ctx) == REDIR_ERROR)
		return (REDIR_ERROR);
	close(fd);
	return (REDIR_OK);
}
