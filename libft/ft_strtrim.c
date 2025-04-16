/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 14:26:25 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/05 16:02:13 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static size_t	ft_strlen(const char *str)
{
	size_t	length;

	length = 0;
	while (*str)
	{
		length++;
		str++;
	}
	return (length);
}

static char	*ft_strchr(const char *s, int c)
{
	while (*s != '\0')
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if (c == '\0')
		return ((char *)s);
	return (NULL);
}

static char	*copy_trimmed_string(const char *s1, size_t start, size_t end)
{
	size_t	new_len;
	size_t	i;
	char	*trimmed_str;

	new_len = end - start;
	trimmed_str = (char *)malloc(new_len + 1);
	if (!trimmed_str)
		return (NULL);
	i = 0;
	while (i < new_len)
	{
		trimmed_str[i] = s1[start + i];
		i++;
	}
	trimmed_str[new_len] = '\0';
	return (trimmed_str);
}

char	*ft_strtrim(const char *s1, const char *set)
{
	size_t	start;
	size_t	end;

	if (!s1)
		return (NULL);
	if (!set)
		return (copy_trimmed_string(s1, 0, ft_strlen(s1)));
	start = 0;
	end = ft_strlen(s1);
	while (s1[start] && ft_strchr(set, s1[start]))
		start++;
	if (s1[start] == '\0')
		return (copy_trimmed_string(s1, start, start));
	while (end > start && ft_strchr(set, s1[end - 1]))
		end--;
	return (copy_trimmed_string(s1, start, end));
}
