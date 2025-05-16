/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 19:39:43 by tching            #+#    #+#             */
/*   Updated: 2025/05/16 20:19:33 by nyoong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *read_line(const char *prompt)
{
	char *line;

	if (prompt)
		line = readline(prompt);
	else
	{
		char cwd[PATH_MAX];
		if (getcwd(cwd, sizeof(cwd)) == NULL)
			ft_strcpy(cwd, "?");
		char full_prompt[PATH_MAX + 4];
		snprintf(full_prompt, sizeof(full_prompt), "%s$ ", cwd);
		line = readline(full_prompt);
	}
	if (line && *line)
		add_history(line);
	return line;
}

int ends_in_pipe(const char *s)
{
	size_t i = ft_strlen(s);
	while (i > 0 && is_whitespace((unsigned char)s[i - 1]))
		--i;
	return (i > 0 && s[i - 1] == '|');
	}

t_list *tokenize_input(const char *line, t_executor_ctx *ctx)
{
	t_list *tokens = lex_input(line);
	if (!tokens)
		ctx->last_exit_status = 0;
	return tokens;
}

t_pipeline *build_pipeline(t_list *tokens, t_executor_ctx *ctx)
{
	t_pipeline *pipeline = parse(tokens);
	if (!pipeline)
		ctx->last_exit_status = 2;
	return pipeline;
}

void execute_and_cleanup(char *line, t_list *tokens, t_pipeline *pipeline, t_executor_ctx *ctx)
{
	execute_pipeline(pipeline, ctx);
	ft_lstclear(&tokens, free_token);
	free_pipeline(pipeline);
	free(line);
}

int main(void)
{
	t_executor_ctx ctx = { .last_exit_status = 0 };

	setup_signal_handlers();
	while (1)
	{
		char *full_line = read_line(NULL);
		if (!full_line)
			exit(ctx.last_exit_status);
		while (ends_in_pipe(full_line))
        {
            char *more = read_line("> ");
            if (!more)
                exit(ctx.last_exit_status);
            char *tmp = ft_strjoin(full_line, "\n");
            free(full_line);
            full_line = ft_strjoin(tmp, more);
            free(tmp);
            free(more);
        }
		t_list *tokens = tokenize_input(full_line, &ctx);
		if (!tokens)
		{
			free(full_line);
			continue;
		}
		t_pipeline *pipeline = build_pipeline(tokens, &ctx);
		if (!pipeline)
		{
			ft_lstclear(&tokens, free_token);
			free(full_line);
			continue;
		}
		execute_and_cleanup(full_line, tokens, pipeline, &ctx);
	}
	rl_clear_history();
	return ctx.last_exit_status;
}
