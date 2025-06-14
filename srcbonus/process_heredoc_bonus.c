/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_heredoc_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 00:51:38 by tching            #+#    #+#             */
/*   Updated: 2025/06/09 21:06:27 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

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
	struct sigaction	sa_q;
	struct sigaction	sa_old;
	char				*line;

	sa_q.sa_handler = SIG_IGN;
	sigemptyset(&sa_q.sa_mask);
	sa_q.sa_flags = 0;
	sigaction(SIGQUIT, &sa_q, &sa_old);
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
	sigaction(SIGQUIT, &sa_old, NULL);
}

int	process_heredoc(t_word *delimiter_word, t_executor_ctx *ctx)
{
	int		fds[2];
	char	*delim;

	pipe(fds);
	delim = resolve_delimiter_word(delimiter_word, ctx);
	read_until_delimiter(delim, fds[1]);
	free(delim);
	close(fds[1]);
	return (fds[0]);
}
