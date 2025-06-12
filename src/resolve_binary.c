/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve_binary.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 23:12:45 by tching            #+#    #+#             */
/*   Updated: 2025/06/13 00:08:50 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*free_full_cmd(char *full_cmd, char **dirs)
{
	ft_split_free(dirs);
	return (full_cmd);
}

char	*free_resolve_from_path_env(char **dirs)
{
	ft_split_free(dirs);
	errno = ENOENT;
	return (NULL);
}
