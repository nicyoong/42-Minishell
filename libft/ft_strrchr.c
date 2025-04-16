/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 13:19:12 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/05 13:19:45 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

char	*ft_strrchr(const char *s, int c)
{
	char	*last_occurrence;
	char	ch;

	last_occurrence = NULL;
	ch = (char)c;
	while (*s)
	{
		if (*s == ch)
			last_occurrence = (char *)s;
		s++;
	}
	if (ch == '\0')
		return ((char *)s);
	if (last_occurrence != NULL)
		return (last_occurrence);
	else
		return (NULL);
}
