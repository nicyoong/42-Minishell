/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 20:51:37 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/08 20:53:56 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h" 

static int	handle_format_specifiers(char specifier, va_list args)
{
	int	printed_chars;

	printed_chars = 0;
	if (specifier == 'c')
		printed_chars += handle_char(args);
	else if (specifier == 's')
		printed_chars += handle_string(args);
	else if (specifier == 'p')
		printed_chars += handle_pointer(args);
	else if (specifier == 'd' || specifier == 'i')
		printed_chars += handle_integer(args);
	else if (specifier == 'u')
		printed_chars += handle_unsigned(args);
	else if (specifier == 'x')
		printed_chars += handle_hex(args, 0);
	else if (specifier == 'X')
		printed_chars += handle_hex(args, 1);
	else if (specifier == '%')
		printed_chars += handle_percent(1);
	return (printed_chars);
}

int	ft_printf(const char *format, ...)
{
	va_list		args;
	int			printed_chars;
	const char	*ptr;

	printed_chars = 0;
	ptr = format;
	va_start(args, format);
	while (*ptr)
	{
		if (*ptr == '%' && *(ptr + 1))
		{
			ptr++;
			printed_chars += handle_format_specifiers(*ptr, args);
		}
		else
		{
			ft_putchar_fd(*ptr, 1);
			printed_chars++;
		}
		ptr++;
	}
	va_end(args);
	return (printed_chars);
}
