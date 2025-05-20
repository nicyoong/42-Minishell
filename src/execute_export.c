/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:24:23 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/20 00:00:33 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;
t_export	*g_export_list = NULL;

void	print_environment(void)
{
	size_t		count;
	t_export	**arr;

	count = count_exports(g_export_list);
	if (count == 0)
		return ;
	arr = list_to_array(g_export_list, count);
	if (!arr)
		return ;
	sort_exports_insertion(arr, count);
	print_exports(arr, count);
	free(arr);
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

int	handle_single_export_arg(const char *arg)
{
	char		*name;
	char		*error_part;
	const char	*eq;
	int			ret;

	eq = ft_strchr(arg, '=');
	ret = 0;
	if (eq)
	{
		name = ft_substr(arg, 0, eq - arg);
		error_part = ft_substr(arg, 0, (eq - arg) + 1);
	}
	else
	{
		name = ft_strdup(arg);
		error_part = ft_strdup(arg);
	}
	if (!is_valid_identifier(name))
	{
		write(2, "export: '", 9);
		write(2, error_part, ft_strlen(error_part));
		write(2, "': not a valid identifier\n", 25);
		ret = 1;
	}
	else if (eq)
		handle_setenv_and_export(name, eq + 1);
	else
		add_export(name, false);
	free(name);
	free(error_part);
	return (ret);
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

int	execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int	save_in;
	int	save_out;
	int	save_err;
	int	ret;

	ret = 0;
	if (g_export_list == NULL)
		init_export_list_from_environ();
	save_stdio(&save_in, &save_out, &save_err);
	if (setup_redirections(redirects, ctx) < 0)
	{
		restore_stdio(save_in, save_out, save_err);
		ctx->last_exit_status = 1;
		return (1);
	}
	if (!argv[1])
		print_environment();
	else
		ret = process_export_args(argv);
	restore_stdio(save_in, save_out, save_err);
	ctx->last_exit_status = ret;
	return (ret);
}
