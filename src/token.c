/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 14:50:10 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/10 18:00:11 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "minishell.h"
#include "parser.h"

t_token	*create_token(t_token_type type)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	token->type = type;
	token->word = NULL;
	if (type == TOKEN_WORD)
	{
		token->word = malloc(sizeof(t_word));
		token->word->segments = NULL;
	}
	return (token);
}

void	add_segment(t_word *word, t_segment_type seg_type, const char *value)
{
	t_segment	*seg;

	seg = malloc(sizeof(t_segment));
	seg->type = seg_type;
	seg->value = ft_strdup(value);
	ft_lstadd_back(&word->segments, ft_lstnew(seg));
}

void	free_token(void *token_ptr)
{
	t_token	*token;

	token = (t_token *)token_ptr;
	if (token->word)
	{
		ft_lstclear(&token->word->segments, (void (*)(void *))free);
		free(token->word);
	}
	free(token);
}
