/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 00:51:38 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 00:35:48 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*resolve_value(t_segment *s, t_executor_ctx *ctx)
{
	char	*val;

	if (s->type == VARIABLE)
	{
		val = getenv(s->value);
		if (!val)
			return (ft_strdup(""));
		return (ft_strdup(val));
	}
	else if (s->type == EXIT_STATUS)
		return (ft_itoa(ctx->last_exit_status));
	else
		return (ft_strdup(s->value));
}

char	*resolve_delimiter_word(t_word *delimiter_word, t_executor_ctx *ctx)
{
	char		buffer[1024];
	char		*resolved;
	t_list		*seg;
	t_segment	*s;

	ft_memset(buffer, 0, sizeof(buffer));
	seg = delimiter_word->segments;
	while (seg)
	{
		s = seg->content;
		resolved = resolve_value(s, ctx);
		ft_strlcat(buffer, resolved, sizeof(buffer));
		free(resolved);
		seg = seg->next;
	}
	return (ft_strdup(buffer));
}

void	read_until_delimiter(const char *delim, int fd_write)
{
	struct sigaction	old_int;
	struct sigaction	old_quit;
	char				*line;

	heredoc_signals_enable(&old_int, &old_quit);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delim) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		write(fd_write, line, ft_strlen(line));
		write(fd_write, "\n", 1);
		free(line);
	}
	heredoc_signals_restore(&old_int, &old_quit);
}

int	process_heredoc(t_word *delimiter_word, t_executor_ctx *ctx)
{
	int		fds[2];
	pid_t	pid;
	char	*delim;
	int		result_fd;

	if (pipe(fds) < 0)
		return (-1);
	delim = resolve_delimiter_word(delimiter_word, ctx);
	pid = fork();
	if (fork_failed_cleanup(pid, delim, fds))
		return (-1);
	if (pid == 0)
		execute_heredoc_child(delim, fds);
	free(delim);
	result_fd = collect_heredoc_parent(pid, fds, ctx);
	return (result_fd);
}
