/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 20:51:42 by nyoong            #+#    #+#             */
/*   Updated: 2024/11/08 20:54:04 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <unistd.h>
# include <stdint.h>
# include "libft.h"

int		ft_putnbr_hex_fd(uintptr_t n, int fd, int uppercase);
void	ft_putnbr_unsigned_fd(unsigned int n, int fd);
int		num_len(int num);
int		num_len_unsigned(unsigned int num);
int		num_len_hex(unsigned int num);
int		handle_char(va_list args);
int		handle_string(va_list args);
int		handle_pointer(va_list args);
int		handle_integer(va_list args);
int		handle_unsigned(va_list args);
int		handle_hex(va_list args, int uppercase);
int		handle_percent(int n);
int		ft_printf(const char *format, ...);

#endif