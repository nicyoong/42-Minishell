/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:15:38 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 01:20:23 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_redir_status	handle_heredoc_redirect(t_redirect *r,
		t_executor_ctx *ctx, int *heredoc_fd)
{
	if (*heredoc_fd != -1)
		close(*heredoc_fd);
	*heredoc_fd = process_heredoc(r->filename, ctx);
	if (*heredoc_fd < 0)
		return (HEREDOC_ABORT);
	return (REDIR_OK);
}

// t_redir_status	handle_regular_redirect(t_redirect *r, t_executor_ctx *ctx)
// {
// 	char	path[4096];
// 	char	*trimmed;
// 	int		fd;

// 	if (build_path_from_word(r->filename, path, sizeof(path), ctx) < 0)
// 	{
// 		ctx->last_exit_status = 1;
// 		return (REDIR_ERROR);
// 	}
// 	trimmed = trim_and_validate_path(path);
// 	if (!trimmed)
// 	{
// 		ctx->last_exit_status = 1;
// 		return (REDIR_ERROR);
// 	}
// 	ft_strncpy(path, trimmed, sizeof(path) - 1);
// 	path[sizeof(path) - 1] = '\0';
// 	free(trimmed);
// 	fd = open_redirection_fd(r->type, path, r->filename, ctx);
// 	if (fd < 0)
// 	{
// 		perror("minishell: redirection open");
// 		ctx->last_exit_status = 1;
// 		return (REDIR_ERROR);
// 	}
// 	if (duplicate_fd(fd, r->type) < 0)
// 	{
// 		ctx->last_exit_status = 1;
// 		close(fd);
// 		return (REDIR_ERROR);
// 	}
// 	close(fd);
// 	return (REDIR_OK);
// }

t_redir_status	setup_final_heredoc(int heredoc_fd, t_executor_ctx *ctx)
{
	if (heredoc_fd < 0)
		return (REDIR_OK);
	if (dup2(heredoc_fd, STDIN_FILENO) < 0)
	{
		perror("minishell: heredoc dup2");
		ctx->last_exit_status = 1;
		close(heredoc_fd);
		return (REDIR_ERROR);
	}
	close(heredoc_fd);
	return (REDIR_OK);
}

t_redir_status	setup_redirections(t_list *redirects,
	t_executor_ctx *ctx, int *heredoc_fd)
{
	t_redirect		*r;
	t_list			*node;
	t_redir_status	st;

	*heredoc_fd = -1;
	node = redirects;
	while (node)
	{
		r = node->content;
		if (r->type == REDIR_HEREDOC)
			st = handle_heredoc_redirect(r, ctx, heredoc_fd);
		else
			st = handle_regular_redirect(r, ctx);
		if (st != REDIR_OK)
			return (st);
		node = node->next;
	}
	return (setup_final_heredoc(*heredoc_fd, ctx));
}
