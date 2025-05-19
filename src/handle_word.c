/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 00:17:07 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/20 00:17:47 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_word	*copy_word(t_word *src)
{
	t_word		*dst;
	t_list		*segments;
	t_list		*tmp;
	t_segment	*src_seg;
	t_segment	*dst_seg;

	dst = ft_calloc(1, sizeof(t_word));
	segments = NULL;
	tmp = src->segments;
	while (tmp)
	{
		src_seg = tmp->content;
		dst_seg = ft_calloc(1, sizeof(t_segment));
		dst_seg->type = src_seg->type;
		dst_seg->value = ft_strdup(src_seg->value);
		ft_lstadd_back(&segments, ft_lstnew(dst_seg));
		tmp = tmp->next;
	}
	dst->segments = segments;
	return (dst);
}

int	process_word(t_list **tokens, t_command *cmd, t_list *head)
{
	t_token	*token;
	t_word	*arg;

	token = head->content;
	arg = copy_word(token->word);
	if (!arg)
	{
		free(head);
		free_command(cmd);
		return (0);
	}
	ft_lstadd_back(&cmd->arguments, ft_lstnew(arg));
	*tokens = head->next;
	free(head);
	return (1);
}
