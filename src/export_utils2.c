/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:24:39 by tching            #+#    #+#             */
/*   Updated: 2025/05/20 13:24:48 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;
t_export	*g_export_list = NULL;

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
			return ;
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

size_t	count_exports(t_export *head)
{
	size_t	count;

	count = 0;
	while (head)
	{
		count++;
		head = head->next;
	}
	return (count);
}

t_export	**list_to_array(t_export *head, size_t count)
{
	t_export	**arr;
	size_t		i;

	arr = malloc(sizeof(*arr) * count);
	if (!arr)
		return (NULL);
	i = 0;
	while (i < count)
	{
		arr[i] = head;
		head = head->next;
		i++;
	}
	return (arr);
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
			if (value)
				printf("declare -x %s=\"%s\"\n", n, value);
			else
				printf("declare -x %s=\"\"\n", n);
		}
		i++;
	}
}