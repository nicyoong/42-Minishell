/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:24:23 by nyoong            #+#    #+#             */
/*   Updated: 2025/05/17 01:54:10 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char **environ;
t_export *g_export_list = NULL;

void save_stdio(int *in, int *out, int *err)
{
	*in  = dup(STDIN_FILENO);
	*out = dup(STDOUT_FILENO);
	*err = dup(STDERR_FILENO);
}

void restore_stdio(int in, int out, int err)
{
	dup2(in,  STDIN_FILENO);
	dup2(out, STDOUT_FILENO);
	dup2(err, STDERR_FILENO);
	close(in);
	close(out);
	close(err);
}

t_export *find_export(const char *name)
{
	t_export	*cur;
	
	cur = g_export_list;
	while (cur)
	{
		if (strcmp(cur->name, name) == 0)
			return (cur);
		cur = cur->next;
	}
	return (NULL);
}

void	add_export(const char *name, bool assigned)
{
	t_export	*ent;

	ent = find_export(name);
	if (ent)
		ent->assigned = ent->assigned || assigned;
	else
	{
		ent = malloc(sizeof(*ent));
		if (!ent)
			return;
		ent->name = ft_strdup(name);
		ent->assigned = assigned;
		ent->next = g_export_list;
		g_export_list = ent;
	}
}

void	init_export_list_from_environ(void)
{
	char	**e;
	char	*eq;
	size_t	namelen;

	e = environ;
	while (*e)
	{
		eq = ft_strchr(*e, '=');
		if (!eq)
		{
			e++;
			continue;
		}
		namelen = eq - *e;
		char *name = ft_strndup(*e, namelen);
		if (!name)
		{
			e++;
			continue;
		}
		add_export(name, true);
		free(name);
		e++;
	}
}

void	remove_export(const char *name)
{
	t_export	**prev;
	t_export	*cur;
	
	prev = &g_export_list;
	while (*prev)
	{
		cur = *prev;
		if (ft_strcmp(cur->name, name) == 0)
		{
			*prev = cur->next;
			free(cur->name);
			free(cur);
			return;
		}
		prev = &cur->next;
	}
}

void	sort_exports_insertion(t_export **arr, size_t n)
{
	size_t		i;
	t_export	*key;
	size_t		j;
	
	i = 1;
	while (i < n)
	{
		key = arr[i];
		j = i;
		while (j > 0 && ft_strcmp(arr[j-1]->name, key->name) > 0)
		{
			arr[j] = arr[j-1];
			--j;
		}
		arr[j] = key;
		++i;
	}
}

void	print_environment(void)
{
	size_t count = 0;
	t_export *e;
	while (e)
	{
		count++;
		e = e->next;
	}
	t_export **arr = malloc(sizeof(*arr) * count);
	if (!arr)
		return;
	size_t i = 0;
	e = g_export_list;
	while (e)
	{
		arr[i++] = e;
		e = e->next;
	}
	sort_exports_insertion(arr, count);
	i = 0;
	while (i < count)
	{
		t_export *e = arr[i];
		const char *n = e->name;

		if (strcmp(n, "LINES") == 0 ||
			strcmp(n, "COLUMNS") == 0 ||
			strcmp(n, "_") == 0)
		{
			i++;
			continue;
		}

		if (!e->assigned)
			printf("declare -x %s\n", n);
		else
			printf("declare -x %s=\"%s\"\n",
				n,
				getenv(n) ? getenv(n) : "");

		i++;
	}
	free(arr);
}

int handle_single_export_arg(const char *arg)
{
	char *name;
	char *error_part;
	const char *eq = strchr(arg, '=');
	int ret = 0;

	if (eq)
	{
		name       = ft_substr(arg, 0, eq - arg);
		error_part = ft_substr(arg, 0, (eq - arg) + 1);
	}
	else
	{
		name       = ft_strdup(arg);
		error_part = ft_strdup(arg);
	}

	if (!is_valid_identifier(name))
	{
		fprintf(stderr, "export: '%s': not a valid identifier\n", error_part);
		ret = 1;
	}
	else if (eq)
	{
		/* assigned form: always setenv */
		if (setenv(name, eq + 1, 1) < 0)
		{
			perror("export");
			ret = 1;
		}
		add_export(name, true);
	}
	else
	{
		/* bare form: just mark for export, no setenv if unset */
		add_export(name, false);
	}

	free(name);
	free(error_part);
	return ret;
}

int process_export_args(char **argv)
{
	int ret = 0;
	for (int i = 1; argv[i]; i++) {
		if (handle_single_export_arg(argv[i]) != 0)
			ret = 1;
	}
	return ret;
}

int	execute_export(char **argv, t_list *redirects, t_executor_ctx *ctx)
{
	int save_in, save_out, save_err;
	int ret = 0;

	if (g_export_list == NULL)
		init_export_list_from_environ();
	save_stdio(&save_in, &save_out, &save_err);
	if (setup_redirections(redirects, ctx) < 0)
	{
		restore_stdio(save_in, save_out, save_err);
		ctx->last_exit_status = 1;
		return 1;
	}
	if (!argv[1])
		print_environment();
	else
		ret = process_export_args(argv);
	restore_stdio(save_in, save_out, save_err);
	ctx->last_exit_status = ret;
	return ret;
}

