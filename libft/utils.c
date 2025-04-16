/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 20:52:13 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/08 21:00:21 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	num_len(int num)
{
	int	len;

	len = 0;
	if (num == 0)
		return (1);
	if (num < 0)
	{
		len++;
		num = -num;
	}
	while (num)
	{
		len++;
		num /= 10;
	}
	return (len);
}

void	ft_putnbr_unsigned_fd(unsigned int n, int fd)
{
	if (n >= 10)
		ft_putnbr_unsigned_fd(n / 10, fd);
	ft_putchar_fd(n % 10 + '0', fd);
}

int	ft_putnbr_hex_fd(uintptr_t num, int fd, int is_upper)
{
	const char	*hex_digits;
	int			count;

	if (is_upper)
		hex_digits = "0123456789ABCDEF";
	else
		hex_digits = "0123456789abcdef";
	count = 0;
	if (num >= 16)
		count += ft_putnbr_hex_fd(num / 16, fd, is_upper);
	ft_putchar_fd(hex_digits[num % 16], fd);
	count++;
	return (count);
}

int	num_len_unsigned(unsigned int num)
{
	int	len;

	len = 0;
	if (num == 0)
		return (1);
	while (num)
	{
		len++;
		num /= 10;
	}
	return (len);
}

int	num_len_hex(unsigned int num)
{
	int	len;

	len = 0;
	if (num == 0)
		return (1);
	while (num)
	{
		len++;
		num /= 16;
	}
	return (len);
}
