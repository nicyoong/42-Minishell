/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:02:19 by tching            #+#    #+#             */
/*   Updated: 2025/05/17 20:23:43 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	append_to_buffer_safely(char *buffer,
			size_t bufsize, const char *resolved)
{
	if (ft_strlen(buffer) + ft_strlen(resolved) >= bufsize)
		return (-1);
	ft_strcat(buffer, resolved);
	return (0);
}

int	build_path_from_word(t_word *word, char *buffer,
			size_t bufsize, t_executor_ctx *ctx)
{
	char		*resolved;
	t_list		*seg;
	t_segment	*s;

	buffer[0] = '\0';
	seg = word->segments;
	while (seg)
	{
		s = seg->content;
		resolved = resolve_segment_value(s, ctx);
		if (!resolved)
			return (-1);
		if (append_to_buffer_safely(buffer, bufsize, resolved) < 0)
		{
			free(resolved);
			return (-1);
		}
		free(resolved);
		seg = seg->next;
	}
	return (0);
}

char	*trim_and_validate_path(const char *path)
{
	char	*trimmed;

	trimmed = ft_strtrim(path, " \t\n\r");
	if (!trimmed || trimmed[0] == '\0')
	{
		if (trimmed)
			free(trimmed);
		return (NULL);
	}
	return (trimmed);
}
