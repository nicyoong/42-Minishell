/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 21:52:37 by nyoong            #+#    #+#             */
/*   Updated: 2025/06/12 22:01:20 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	count_envp_entries(t_export *head)
{
	size_t	n;

	n = 0;
	while (head)
	{
		if (head->assigned)
			n++;
		head = head->next;
	}
	return (n);
}

static char	*make_env_string(const char *name, const char *value)
{
	size_t	nlen;
	size_t	vlen;
	char	*s;

	nlen = ft_strlen(name);
	vlen = ft_strlen(value);
	s = malloc(nlen + 1 + vlen + 1);
	if (!s)
		return (NULL);
	ft_memcpy(s, name, nlen);
	s[nlen] = '=';
	ft_memcpy(s + nlen + 1, value, vlen);
	s[nlen + 1 + vlen] = '\0';
	return (s);
}

static void	free_envp_array(char **envp, size_t used)
{
	size_t	i;

	i = 0;
	while (i < used)
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}

static bool	populate_envp(char **envp, t_export *head)
{
	size_t		i;
	t_export	*cur;

	i = 0;
	cur = head;
	while (cur)
	{
		if (cur->assigned)
		{
			envp[i] = make_env_string(cur->name, cur->value);
			if (!envp[i])
			{
				free_envp_array(envp, i);
				return (false);
			}
			i++;
		}
		cur = cur->next;
	}
	envp[i] = NULL;
	return (true);
}

char	**export_list_to_envp(t_executor_ctx *ctx)
{
	size_t	count;
	char	**envp;

	count = count_envp_entries(ctx->export_list);
	envp = malloc((count + 1) * sizeof (char *));
	if (!envp)
		return (NULL);
	if (!populate_envp(envp, ctx->export_list))
		return (NULL);
	return (envp);
}
