/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 11:46:49 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/05 11:46:50 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	const char	*src_start = src;
	size_t		src_len;
	size_t		copy_index;

	src_len = 0;
	while (*src++)
		src_len++;
	if (size == 0)
		return (src_len);
	src = src_start;
	copy_index = 0;
	while (copy_index < size - 1 && *src)
	{
		*dest++ = *src++;
		copy_index++;
	}
	*dest = '\0';
	return (src_len);
}
