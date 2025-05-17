/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils5.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:06:27 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:07:06 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_segment(void *seg_ptr)
{
	t_segment	*seg;

	seg = seg_ptr;
	free(seg->value);
	free(seg);
}

void	free_word(void *word_ptr)
{
	t_word	*w;

	w = word_ptr;
	ft_lstclear(&w->segments, free_segment);
	free(w);
}

void	free_redirect(void *redir_ptr)
{
	t_redirect	*r;

	r = redir_ptr;
	free_word(r->filename);
	free(r);
}

void	free_command(void *cmd_ptr)
{
	t_command	*cmd;

	cmd = cmd_ptr;
	ft_lstclear(&cmd->arguments, free_word);
	ft_lstclear(&cmd->redirects, free_redirect);
	free(cmd);
}

void	free_pipeline(t_pipeline *pipeline)
{
	ft_lstclear(&pipeline->commands, free_command);
	free(pipeline);
}
