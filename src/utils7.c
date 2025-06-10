/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils7.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 20:45:51 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/10 20:50:01 by nyoong           ###   ########.fr       */
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
