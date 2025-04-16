/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 20:51:56 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/08 20:54:35 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	handle_char(va_list args)
{
	char	c;

	c = (char)va_arg(args, int);
	ft_putchar_fd(c, 1);
	return (1);
}

int	handle_string(va_list args)
{
	char	*str;

	str = va_arg(args, char *);
	if (str)
	{
		ft_putstr_fd(str, 1);
		return (ft_strlen(str));
	}
	else
	{
		ft_putstr_fd("(null)", 1);
		return (6);
	}
}

int	handle_pointer(va_list args)
{
	void	*ptr_val;
	int		count;

	count = 0;
	ptr_val = va_arg(args, void *);
	if (ptr_val == NULL)
	{
		ft_putstr_fd("(nil)", 1);
		return (5);
	}
	ft_putstr_fd("0x", 1);
	count += 2;
	count += ft_putnbr_hex_fd((uintptr_t)ptr_val, 1, 0);
	return (count);
}

int	handle_percent(int n)
{
	(void)n;
	ft_putchar_fd('%', 1);
	return (1);
}
