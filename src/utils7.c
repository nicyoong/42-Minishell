/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils7.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 20:45:51 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/10 23:46:47 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_export_list(t_export *head)
{
	t_export	*cur;
	t_export	*next;

	cur = head;
	while (cur)
	{
		next = cur->next;
		free(cur->name);
		free(cur);
		cur = next;
	}
}

int	handle_pipeline_failure(t_pipeline *pipeline,
	t_list *tokens, char *full_line)
{
	if (!pipeline)
	{
		ft_lstclear(&tokens, free_token);
		free(full_line);
		return (0);
	}
	return (1);
}
