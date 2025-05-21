/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:24:39 by tching            #+#    #+#             */
/*   Updated: 2025/05/21 13:30:34 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		while (j > 0 && ft_strcmp(arr[j - 1]->name, key->name) > 0)
		{
			arr[j] = arr[j - 1];
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

char	*parse_export_arg(const char *arg, char **name, char **error_part)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		*name = ft_substr(arg, 0, eq - arg);
		*error_part = ft_substr(arg, 0, (eq - arg) + 1);
	}
	else
	{
		*name = ft_strdup(arg);
		*error_part = ft_strdup(arg);
	}
	return (eq);
}

int	validate_export_identifier(const char *name, const char *error_part)
{
	if (!is_valid_identifier(name))
	{
		write(2, "export: '", 9);
		write(2, error_part, ft_strlen(error_part));
		write(2, "': not a valid identifier\n", 25);
		return (0);
	}
	return (1);
}
