/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve_segment.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 00:04:18 by tching            #+#    #+#             */
/*   Updated: 2025/05/17 00:04:48 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
