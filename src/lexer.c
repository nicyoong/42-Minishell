/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:16:48 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:25:09 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	skip_whitespace(const char *input, int i)
{
	while (input[i] && is_whitespace(input[i]))
		i++;
	return (i);
}

int	handle_operator(const char *input, int *i, t_list **tokens)
{
	t_token_type	type;

	type = get_operator(input, i);
	ft_lstadd_back(tokens, ft_lstnew(create_token(type)));
	return (*i);
}

int	handle_word(const char *input, int *i, int len, t_list **tokens)
{
	t_token	*token;
	char	quote_type;

	token = create_token(TOKEN_WORD);
	if (!token)
		return (0);
	while (*i < len && !is_whitespace(input[*i])
		&& !is_operator_char(input[*i]))
	{
		if (input[*i] == '\'' || input[*i] == '"')
		{
			quote_type = input[*i];
			if (!process_quoted_content(input, i, quote_type, token->word))
			{
				free_token(token);
				return (0);
			}
		}
		else
			process_unquoted_segment(input, i, token->word);
	}
	ft_lstadd_back(tokens, ft_lstnew(token));
	return (1);
}

t_list	*lex_input(const char *input)
{
	t_list	*tokens;
	int		i;
	int		len;

	tokens = NULL;
	i = 0;
	len = ft_strlen(input);
	while (i < len)
	{
		i = skip_whitespace(input, i);
		if (i >= len)
			break ;
		if (is_operator_char(input[i]))
			handle_operator(input, &i, &tokens);
		else
		{
			if (!handle_word(input, &i, len, &tokens))
			{
				ft_lstclear(&tokens, free_token);
				return (NULL);
			}
		}
	}
	return (tokens);
}
