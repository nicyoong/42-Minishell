#include "minishell.h"

void	flush_buffer(t_word *word, char *buffer, int *buf_idx)
{
	if (*buf_idx > 0)
	{
		buffer[*buf_idx] = '\0';
		add_segment(word, LITERAL, buffer);
		*buf_idx = 0;
	}
}

void	parse_exit_status(const char *input, int *i, t_word *word)
{
	(void) input;
	add_segment(word, EXIT_STATUS, "$?");
	(*i)++;
}

void	parse_variable_name(const char *input, int *i, t_word *word)
{
	char	var[1024];
	int		var_idx;

	var_idx = 0;
	while (is_valid_var_char(input[*i]))
		var[var_idx++] = input[(*i)++];
	var[var_idx] = '\0';
	add_segment(word, VARIABLE, var);
}

void	handle_variable_expansion(const char *input, int *i, t_word *word, char *buffer, int *buf_idx)
{
	flush_buffer(word, buffer, buf_idx);
	(*i)++;
	if (input[*i] == '?')
		parse_exit_status(input, i, word);
	else if (is_valid_var_char(input[*i]))
		parse_variable_name(input, i, word);
	else
		add_segment(word, LITERAL, "$");
}

int	process_quoted_content(const char *input, int *i, char quote_type, t_word *word)
{
	char	buffer[1024];
	int		buf_idx;

	buf_idx = 0;
	(*i)++;
	while (input[*i] && input[*i] != quote_type)
	{
		if (quote_type == '"' && input[*i] == '$')
			handle_variable_expansion(input, i, word, buffer, &buf_idx);
		else
			buffer[buf_idx++] = input[(*i)++];
	}
	if (buf_idx > 0)
	{
		buffer[buf_idx] = '\0';
		add_segment(word, LITERAL, buffer);
	}
	if (input[*i] != quote_type)
		return (0);
	(*i)++;
	return (1);
}

// char	decode_escape(const char *s, int *idx)
// {
// 	char	c;

// 	c = s[*idx];
// 	if (c == 'n')
// 	{
// 		(*idx)++;
// 		return ('\n');
// 	}
// 	if (c == 't')
// 	{
// 		(*idx)++;
// 		return ('\t');
// 	}
// 	if (c == 'x' && ft_isxdigit(s[*idx+1]) && ft_isxdigit(s[*idx+2]))
// 	{
// 		char hex[3] = { s[*idx+1], s[*idx+2], '\0' };
// 		*idx += 3;
// 		return ((char)hex_to_long(hex));
// 	}
// 	if (c == '\\')
// 	{
// 		(*idx)++;
// 		return ('\\');
// 	}
// 	if (c == '\'')
// 	{
// 		(*idx)++;
// 		return ('\'');
// 	}
// 	(*idx)++;
// 	return (s[*idx - 1]);
// }

char	decode_newline(int *idx)
{
	(*idx)++;
	return ('\n');
}

char	decode_tab(int *idx)
{
	(*idx)++;
	return ('\t');
}

int	is_hex_escape(const char *s, int idx)
{
	return (s[idx] == 'x'
		&& ft_isxdigit(s[idx + 1])
		&& ft_isxdigit(s[idx + 2]));
}

char	decode_hex(const char *s, int *idx)
{
	char	hex[3];

	hex[0] = s[*idx + 1];
	hex[1] = s[*idx + 2];
	hex[2] = '\0';
	*idx += 3;
	return ((char)hex_to_long(hex));
}

char	decode_backslash(int *idx)
{
	(*idx)++;
	return ('\\');
}

char	decode_quote(int *idx)
{
	(*idx)++;
	return ('\'');
}

char	decode_default(const char *s, int *idx)
{
	(*idx)++;
	return (s[*idx - 1]);
}


char	decode_escape(const char *s, int *idx)
{
	char	c;

	c = s[*idx];
	if (c == 'n')
		return (decode_newline(idx));
	if (c == 't')
		return (decode_tab(idx));
	if (is_hex_escape(s, *idx))
		return (decode_hex(s, idx));
	if (c == '\\')
		return (decode_backslash(idx));
	if (c == '\'')
		return (decode_quote(idx));
	return (decode_default(s, idx));
}

void	process_ansi_c_quote(const char *input, int *i, t_word *word)
{
	char buffer[1024];
	size_t buf_idx;

	buf_idx = 0;
	*i += 2;
	while (input[*i] && input[*i] != '\'')
	{
		if (input[*i] == '\\')
		{
			(*i)++;
			char decoded = decode_escape(input, i);
			buffer[buf_idx++] = decoded;
		}
		else
			buffer[buf_idx++] = input[(*i)++];
		if (buf_idx >= sizeof(buffer)-1)
			break;
	}
	buffer[buf_idx] = '\0';
	add_segment(word, LITERAL, buffer);
	if (input[*i] == '\'')
		(*i)++;
}

void	process_unquoted_segment(const char *input, int *i, t_word *word)
{
		char	buffer[1024];
		int		buf_idx;

		buf_idx = 0;
		while (input[*i]
			&& !is_whitespace(input[*i])
			&& !is_operator_char(input[*i])
			&& input[*i] != '\''
			&& input[*i] != '"')
		{
			if (input[*i] == '$' && input[*i + 1] == '\'')
			{
				if (buf_idx > 0)
				{
					buffer[buf_idx] = '\0';
					add_segment(word, LITERAL, buffer);
					buf_idx = 0;
				}
				process_ansi_c_quote(input, i, word);
			}
			else if (input[*i] == '$')
			{
				if (buf_idx > 0) {
					buffer[buf_idx] = '\0';
					add_segment(word, LITERAL, buffer);
					buf_idx = 0;
				}
				handle_variable_expansion(input, i, word, buffer, &buf_idx);
			}
			else
				buffer[buf_idx++] = input[(*i)++];
		}
		if (buf_idx > 0)
		{
			buffer[buf_idx] = '\0';
			add_segment(word, LITERAL, buffer);
		}
}

t_token_type	get_operator(const char *input, int *i)
{
	if (input[*i] == '|')
	{
		(*i)++;
		return (TOKEN_PIPE);
	}
	else if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			(*i) += 2;
			return (TOKEN_REDIRECT_HEREDOC);
		}
		(*i)++;
		return (TOKEN_REDIRECT_IN);
	}
	else if (input[*i] == '>')
	{
		if (input[*i + 1] == '>')
		{
			(*i) += 2;
			return (TOKEN_REDIRECT_APPEND);
		}
		(*i)++;
		return (TOKEN_REDIRECT_OUT);
	}
	return (TOKEN_WORD);
}

int	skip_whitespace(const char *input, int i)
{
	while (input[i] && is_whitespace(input[i]))
		i++;
	return (i);
}

int	handle_operator(const char *input, int *i, t_list **tokens)
{
	t_token_type	type;

	type = get_operator(input, i);
	ft_lstadd_back(tokens, ft_lstnew(create_token(type)));
	return (*i);
}

int	handle_word(const char *input, int *i, int len, t_list **tokens)
{
	t_token *token;

	token = create_token(TOKEN_WORD);
	if (!token)
		return (0);
	while (*i < len && !is_whitespace(input[*i])
		 && !is_operator_char(input[*i]))
	{
		if (input[*i] == '\'' || input[*i] == '"')
		{
			char quote_type = input[*i];
			if (!process_quoted_content(input, i, quote_type, token->word))
			{
				free_token(token);
				return (0);
			}
		}
		else
			process_unquoted_segment(input, i, token->word);
	}
	ft_lstadd_back(tokens, ft_lstnew(token));
	return (1);
}

t_list	*lex_input(const char *input)
{
	t_list	*tokens;
	int		i;
	int		len;

	tokens = NULL;
	i = 0;
	len = strlen(input);
	while (i < len)
	{
		i = skip_whitespace(input, i);
		if (i >= len)
			break;
		if (is_operator_char(input[i]))
			handle_operator(input, &i, &tokens);
		else
		{
			if (!handle_word(input, &i, len, &tokens))
			{
				ft_lstclear(&tokens, free_token);
				return (NULL);
			}
		}
	}
	return (tokens);
}
