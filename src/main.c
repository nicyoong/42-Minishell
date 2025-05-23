/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 19:39:43 by tching            #+#    #+#             */
/*   Updated: 2025/05/21 15:11:39 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*read_line(void)
{
	char	cwd[1024];
	char	prompt[1024 + 3];
	char	*line;
	char	*full;

	if (!getcwd(cwd, sizeof(cwd)))
		return (NULL);
	ft_strcpy(prompt, cwd);
	ft_strcat(prompt, "$ ");
	line = readline(prompt);
	if (!line)
		return (NULL);
	if (*line)
		add_history(line);
	full = ft_strdup(line);
	free(line);
	full = read_continued_input(full);
	return (full);
}

t_list	*tokenize_input(const char *line, t_executor_ctx *ctx)
{
	t_list	*tokens;

	tokens = lex_input(line);
	if (!tokens)
		ctx->last_exit_status = 0;
	return (tokens);
}

void	execute_and_cleanup(char *line, t_list *tokens,
		t_pipeline *pipeline, t_executor_ctx *ctx)
{
	execute_pipeline(pipeline, ctx);
	ft_lstclear(&tokens, free_token);
	free_pipeline(pipeline);
	free(line);
}

static void	run_shell_loop(t_executor_ctx *ctx)
{
	t_list		*tokens;
	t_pipeline	*pipeline;
	char		*full_line;

	while (1)
	{
		full_line = read_line();
		if (!full_line)
			exit(ctx->last_exit_status);
		tokens = tokenize_input(full_line, ctx);
		if (!tokens)
		{
			free(full_line);
			continue ;
		}
		pipeline = build_pipeline(tokens, ctx);
		if (!pipeline)
		{
			ft_lstclear(&tokens, free_token);
			free(full_line);
			continue ;
		}
		execute_and_cleanup(full_line, tokens, pipeline, ctx);
	}
}

int	main(void)
{
	t_executor_ctx	ctx;

	ctx.last_exit_status = 0;
	set_executor_ctx(&ctx);
	setup_signal_handlers();
	run_shell_loop(&ctx);
	rl_clear_history();
	return (ctx.last_exit_status);
}
