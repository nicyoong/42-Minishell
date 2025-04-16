/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:58:39 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/06 18:07:54 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static char	*ft_strdup(const char *s1)
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

static int	get_num_length(int n)
{
	int	length;

	length = 0;
	if (n <= 0)
		length = 1;
	while (n != 0)
	{
		n /= 10;
		length++;
	}
	return (length);
}

static char	*convert_to_string(unsigned int num, int len, int is_negative)
{
	char	*str;
	int		index;

	str = (char *)malloc(len + 1);
	if (!str)
		return (NULL);
	str[len] = '\0';
	if (is_negative)
		str[0] = '-';
	index = len - 1;
	while (index >= 0)
	{
		if (index == 0 && is_negative)
			break ;
		str[index] = (num % 10) + '0';
		num /= 10;
		index--;
	}
	return (str);
}

char	*ft_itoa(int n)
{
	int				len;
	unsigned int	num;

	if (n == -2147483648)
	{
		return (ft_strdup("-2147483648"));
	}
	len = get_num_length(n);
	if (n < 0)
		num = -n;
	else
		num = n;
	return (convert_to_string(num, len, n < 0));
}
