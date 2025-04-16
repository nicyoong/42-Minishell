/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 21:35:27 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/06 15:40:23 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

char	*ft_strdup(const char *s1)
{
	const char	*start;
	size_t		length;
	char		*copy;
	char		*copy_start;

	start = s1;
	while (*s1)
		s1++;
	length = s1 - start;
	copy = (char *)malloc((length + 1) * sizeof(char));
	if (!copy)
		return (NULL);
	copy_start = copy;
	while (*start)
		*copy_start++ = *start++;
	*copy_start = '\0';
	return (copy);
}
