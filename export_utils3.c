/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 20:15:27 by tching            #+#    #+#             */
/*   Updated: 2025/05/20 21:24:56 by tiara            ###   ########.fr       */
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
			value = getenv(n);
			print_export_value(value, n);
		}
		i++;
	}
}

int	handle_setenv_and_export(const char *name, const char *value)
{
	if (setenv(name, value, 1) < 0)
	{
		perror("export");
		return (1);
	}
	add_export(name, true);
	return (0);
}

int	process_export_args(char **argv)
{
	int	ret;
	int	i;

	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (handle_single_export_arg(argv[i]) != 0)
			ret = 1;
		i++;
	}
	return (ret);
}
