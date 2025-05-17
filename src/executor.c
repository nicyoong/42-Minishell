#include "minishell.h"

extern char **environ;

int open_redirection_fd(t_redirect_type type, const char *path, t_word *filename, t_executor_ctx *ctx)
{
	int fd = -1;
	int flags;
	mode_t mode = 0644;

	switch (type)
	{
		case REDIR_IN:
			fd = open(path, O_RDONLY);
			break;
		case REDIR_OUT:
			flags = O_WRONLY | O_CREAT | O_TRUNC;
			fd = open(path, flags, mode);
			break;
		case REDIR_APPEND:
			flags = O_WRONLY | O_CREAT | O_APPEND;
			fd = open(path, flags, mode);
			break;
		case REDIR_HEREDOC:
			fd = process_heredoc(filename, ctx);
			break;
		default:
			fd = -1;
			break;
	}
	return fd;
}