/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 22:35:05 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 23:59:04 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_unset(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int			ret;
	int			i;
	const char	*empty_error;

	ret = 0;
	i = 1;
	empty_error = "";
	(void)redirects;
	while (argv[i] != NULL)
	{
		if (!validate_export_identifier(argv[i], empty_error))
		{
			ft_putstr_fd("unset: `", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			ret = 1;
		}
		else
			remove_export(ctx, argv[i]);
		i++;
	}
	ctx->last_exit_status = ret;
	return (ret);
}
