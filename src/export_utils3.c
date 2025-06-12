/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 20:15:27 by tching            #+#    #+#             */
/*   Updated: 2025/06/12 21:46:29 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_export_value(char *value, const char *n)
{
	if (value)
		printf("declare -x %s=\"%s\"\n", n, value);
	else
		printf("declare -x %s=\"\"\n", n);
}

void	print_exports(t_export **arr, size_t count)
{
	size_t		i;
	t_export	*e;
	const char	*n;
	char		*value;

	i = 0;
	while (i < count)
	{
		e = arr[i];
		n = e->name;
		if (ft_strcmp(n, "LINES") == 0 || ft_strcmp(n, "COLUMNS") == 0
			|| ft_strcmp(n, "_") == 0)
		{
			i++;
			continue ;
		}
		if (!e->assigned)
			printf("declare -x %s\n", n);
		else
		{
			value = e->value;
			print_export_value(value, n);
		}
		i++;
	}
}

int	handle_setenv_and_export(t_executor_ctx *ctx,
								const char *name, const char *value)
{
	add_export(ctx, name, value, true);
	return (0);
}

int	handle_single_export_arg(t_executor_ctx *ctx, const char *arg)
{
	char		*name;
	char		*error_part;
	const char	*eq;
	int			ret;

	eq = parse_export_arg(arg, &name, &error_part);
	ret = 0;
	if (!validate_export_identifier(name, error_part))
		ret = 1;
	else if (eq)
		handle_setenv_and_export(ctx, name, eq + 1);
	else
		add_export(ctx, name, NULL, false);
	free(name);
	free(error_part);
	return (ret);
}

int	process_export_args(char **argv, t_executor_ctx *ctx)
{
	int	ret;
	int	i;

	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (handle_single_export_arg(ctx, argv[i]) != 0)
			ret = 1;
		i++;
	}
	return (ret);
}
