/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve_segment_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 00:04:18 by tching            #+#    #+#             */
/*   Updated: 2025/05/27 03:02:11 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

char	*resolve_segment(t_segment *seg, t_executor_ctx *ctx)
{
	char	*value;

	if (seg->type == VARIABLE)
	{
		value = getenv(seg->value);
		if (value)
			return (ft_strdup(value));
		else
			return (ft_strdup(""));
	}
	else if (seg->type == EXIT_STATUS)
		return (ft_itoa(ctx->last_exit_status));
	return (ft_strdup(seg->value));
}

char	*resolve_segment_value(t_segment *s, t_executor_ctx *ctx)
{
	char	*value;

	if (s->type == VARIABLE)
	{
		value = getenv(s->value);
		if (value != NULL)
			return (value);
		else
			return ("");
	}
	else if (s->type == EXIT_STATUS)
		return (ft_itoa(ctx->last_exit_status));
	else
		return (ft_strdup(s->value));
}
