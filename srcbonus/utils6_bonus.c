/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils6_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 14:27:36 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/27 03:02:35 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell_bonus.h"

char	*ft_strcpy(char *dst, const char *src)
{
	char	*orig;

	orig = dst;
	while (*src)
		*dst++ = *src++;
	*dst = '\0';
	return (orig);
}

int	quotes_are_balanced(const char *s)
{
	int		i;
	char	in_quote;

	i = 0;
	in_quote = 0;
	while (s[i])
	{
		if ((s[i] == '"' || s[i] == '\'')
			&& (i == 0 || s[i - 1] != '\\'))
		{
			if (!in_quote)
				in_quote = s[i];
			else if (in_quote == s[i])
				in_quote = 0;
		}
		i++;
	}
	return (!in_quote);
}

char	*read_continued_input(char *full)
{
	char	*line;
	char	*tmp;

	while (!quotes_are_balanced(full))
	{
		line = readline("> ");
		if (!line)
			break ;
		if (*line)
			add_history(line);
		tmp = ft_strjoin(full, "\n");
		free(full);
		full = tmp;
		tmp = ft_strjoin(full, line);
		free(full);
		full = tmp;
		free(line);
	}
	return (full);
}

int	redirect_path_msg(void)
{
	write(STDERR_FILENO, "minishell: redirect error: path too long\n", 41);
	return (-1);
}

int	redirect_invalid_msg(void)
{
	write(STDERR_FILENO, "minishell: redirect error: invalid filename\n", 44);
	return (-1);
}
