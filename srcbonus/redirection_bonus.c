s/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:15:38 by tching            #+#    #+#             */
/*   Updated: 2025/05/24 00:46:54 by tiara            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

static int	handle_heredoc_redirect(t_redirect *r,
		t_executor_ctx *ctx, int *heredoc_fd)
{
	if (*heredoc_fd != -1)
		close(*heredoc_fd);
	*heredoc_fd = process_heredoc(r->filename, ctx);
	if (*heredoc_fd < 0)
		return (-1);
	return (0);
}

static int	handle_regular_redirect(t_redirect *r, t_executor_ctx *ctx)
{
	char	path[4096];
	char	*trimmed;
	int		fd;
	int		ret;

	ret = 0;
	if (build_path_from_word(r->filename, path, sizeof(path), ctx) < 0)
		return (ret = redirect_path_msg());
	trimmed = trim_and_validate_path(path);
	if (!trimmed)
		return (ret = redirect_invalid_msg());
	ft_strncpy(path, trimmed, sizeof(path) - 1);
	path[sizeof(path) - 1] = '\0';
	free(trimmed);
	fd = open_redirection_fd(r->type, path, r->filename, ctx);
	if (fd < 0)
	{
		perror("minishell: redirection open");
		return (-1);
	}
	if (duplicate_fd(fd, r->type) < 0)
		return (-1);
	return (0);
}

static int	setup_final_heredoc(int heredoc_fd)
{
	if (heredoc_fd != -1)
	{
		if (dup2(heredoc_fd, STDIN_FILENO) < 0)
		{
			perror("minishell: heredoc dup2");
			return (-1);
		}
		close(heredoc_fd);
	}
	return (0);
}

int	setup_redirections(t_list *redirects, t_executor_ctx *ctx)
{
	t_redirect	*r;
	int			heredoc_fd;
	t_list		*node;

	heredoc_fd = -1;
	node = redirects;
	while (node != NULL)
	{
		r = node->content;
		if (r->type == REDIR_HEREDOC)
		{
			if (handle_heredoc_redirect(r, ctx, &heredoc_fd) < 0)
				return (-1);
		}
		else
		{
			if (handle_regular_redirect(r, ctx) < 0)
				return (-1);
		}
		node = node->next;
	}
	return (setup_final_heredoc(heredoc_fd));
}
