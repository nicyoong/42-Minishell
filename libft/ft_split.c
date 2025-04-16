/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:40:07 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/05 16:41:55 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static int	count_words(const char *s, char c)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c && in_word == 0)
		{
			in_word = 1;
			count++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

static char	*malloc_word(const char *s, size_t start, size_t end)
{
	char	*word;
	size_t	i;

	word = (char *)malloc(end - start + 1);
	if (!word)
		return (NULL);
	i = 0;
	while (start < end)
		word[i++] = s[start++];
	word[i] = '\0';
	return (word);
}

static int	find_words(const char *s, char c, char **result)
{
	size_t	i;
	size_t	word_start;
	int		word_index;

	i = 0;
	word_start = 0;
	word_index = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			if (word_start == 0 && (i == 0 || s[i - 1] == c))
				word_start = i;
			if ((s[i + 1] == c || s[i + 1] == '\0') && word_start < i + 1)
			{
				result[word_index] = malloc_word(s, word_start, i + 1);
				if (!result[word_index])
					return (word_index);
				word_index++;
				word_start = 0;
			}
		}
		i++;
	}
	return (word_index);
}

static char	**fill_split_array(const char *s, char c, int word_count)
{
	char	**result;
	int		allocated_words;

	result = (char **)malloc((word_count + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	allocated_words = find_words(s, c, result);
	if (allocated_words < word_count)
	{
		while (allocated_words-- > 0)
			free(result[allocated_words]);
		free(result);
		return (NULL);
	}
	result[word_count] = NULL;
	return (result);
}

char	**ft_split(const char *s, char c)
{
	int		word_count;

	if (!s)
		return (NULL);
	word_count = count_words(s, c);
	return (fill_split_array(s, c, word_count));
}
