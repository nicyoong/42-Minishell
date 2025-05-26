/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert_arguments_bonus.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 01:09:41 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 03:00:35 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

char	*get_segment_value(t_segment *s, t_executor_ctx *ctx)
{
	char	*val;

	if (s->type == VARIABLE)
	{
		val = getenv(s->value);
		if (val)
			return (val);
		else
			return ("");
	}
	else if (s->type == EXIT_STATUS)
		return (ft_itoa(ctx->last_exit_status));
	else
		return (s->value);
}

char	*concatenate_segments(t_word *word, t_executor_ctx *ctx)
{
	char		*val;
	char		buffer[1024];
	t_list		*seg;
	t_segment	*s;

	ft_memset(buffer, 0, sizeof(buffer));
	seg = word->segments;
	while (seg)
	{
		s = seg->content;
		val = get_segment_value(s, ctx);
		ft_strcat(buffer, val);
		if (s->type == EXIT_STATUS)
			free(val);
		seg = seg->next;
	}
	return (ft_strdup(buffer));
}

char	**convert_arguments(t_list *args, t_executor_ctx *ctx)
{
	int		i;
	char	**argv;
	char	*arg;
	t_list	*node;
	t_word	*word;

	argv = ft_calloc((ft_lstsize(args)) + 1, sizeof(char *));
	i = 0;
	node = args;
	while (node)
	{
		word = node->content;
		arg = concatenate_segments(word, ctx);
		if (arg[0] != '\0')
			argv[i++] = arg;
		else
			free(arg);
		node = node->next;
	}
	return (argv);
}
