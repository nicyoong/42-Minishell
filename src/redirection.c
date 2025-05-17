/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:15:38 by tching            #+#    #+#             */
/*   Updated: 2025/05/17 22:22:06 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// int setup_redirections(t_list *redirects, t_executor_ctx *ctx)
// {
// 	char path[4096];

// 	for (t_list *node = redirects; node; node = node->next)
// 	{
// 		t_redirect *r = node->content;

// 		if (build_path_from_word(r->filename, path, sizeof(path), ctx) < 0)
// 		{
// 			fprintf(stderr, "redirection error: path too long\n");
// 			return -1;
// 		}
// 		char *trimmed = trim_and_validate_path(path);
// 		if (!trimmed)
// 		{
// 			fprintf(stderr, "redirection error: invalid filename\n");
// 			return -1;
// 		}
// 		ft_strncpy(path, trimmed, sizeof(path) - 1);
// 		path[sizeof(path) - 1] = '\0';
// 		free(trimmed);

// 		int fd = open_redirection_fd(r->type, path, r->filename, ctx);
// 		if (fd < 0)
// 		{
// 			perror("redirection error");
// 			return -1;
// 		}
// 		if (duplicate_fd(fd, r->type) < 0)
// 			return -1;
// 	}
// 	return 0;
// }

static int	report_path_error(void)
{
	printf("redirection error: path too long\n");
	return (-1);
}

static int	report_invalid_filename(void)
{
	printf("redirection error: invalid filename\n");
	return (-1);
}

static int	handle_redirection_path(t_redirect *r, char *path,
		size_t path_size, t_executor_ctx *ctx)
{
	char	*trimmed;

	if (build_path_from_word(r->filename, path, path_size, ctx) < 0)
		return (report_path_error());
	trimmed = trim_and_validate_path(path);
	if (!trimmed)
		return (report_invalid_filename());
	ft_strncpy(path, trimmed, path_size - 1);
	path[path_size - 1] = '\0';
	free(trimmed);
	return (0);
}

static int	process_single_redirection(t_redirect *r,
		char *path, t_executor_ctx *ctx)
{
	int	fd;

	fd = open_redirection_fd(r->type, path, r->filename, ctx);
	if (fd < 0)
	{
		perror("redirection error");
		return (-1);
	}
	if (duplicate_fd(fd, r->type) < 0)
		return (-1);
	return (0);
}

int	setup_redirections(t_list *redirects, t_executor_ctx *ctx)
{
	char		path[4096];
	t_list		*node;
	t_redirect	*r;

	node = redirects;
	while (node)
	{
		r = node->content;
		if (handle_redirection_path(r, path, sizeof(path), ctx) < 0)
			return (-1);
		if (process_single_redirection(r, path, ctx) < 0)
			return (-1);
		node = node->next;
	}
	return (0);
}
