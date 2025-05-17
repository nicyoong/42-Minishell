/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decode1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 23:17:22 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 23:18:55 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	decode_newline(int *idx)
{
	(*idx)++;
	return ('\n');
}

char	decode_tab(int *idx)
{
	(*idx)++;
	return ('\t');
}

char	decode_backslash(int *idx)
{
	(*idx)++;
	return ('\\');
}
