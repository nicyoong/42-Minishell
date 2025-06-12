# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nyoong <nyoong@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/18 18:33:55 by tching            #+#    #+#              #
#    Updated: 2025/06/13 00:06:44 by nyoong           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# compiler & flags
CC      = cc
AR      = ar
CFLAGS  = -Wall -Wextra -Werror -Iinc -Ilibft
LDFLAGS = -Llibft -lft
EXTRAS	= -lreadline

# directory path
SRCDIR  = src
OBJDIR  = obj
LIBDIR  = libft
INCDIR  = inc

# sources
SRCS = $(addprefix $(SRCDIR)/, \
			cd_utils1.c \
			convert_arguments.c \
			decode1.c \
			decode2.c \
			env_utils.c \
			execute_builtin.c \
			execute_cd.c \
			execute_child.c \
			execute_echo.c \
			execute_env.c \
			execute_exit.c \
			execute_export.c \
			execute_pipeline.c \
			execute_pwd.c \
			execute_unset.c \
			expand_variable.c \
			export_utils1.c \
			export_utils2.c \
			export_utils3.c \
			export_utils4.c \
			get_operator.c \
			handle_child_error.c \
			handle_redirect.c \
			handle_token.c \
			handle_word.c \
			heredoc_utils.c \
			lexer.c \
			main.c \
			parser.c \
			process_heredoc.c \
			process_quoted.c \
			process_unquoted.c \
			redirection.c \
			redirection_utils1.c \
			redirection_utils2.c \
			redirection_utils3.c \
			redirection_utils4.c \
			redirection_utils5.c \
			resolve_binary.c \
			resolve_segment.c \
			resolve_utils.c \
			shlvl.c \
			signal.c \
			token.c \
			utils1.c \
			utils2.c \
			utils3.c \
			utils4.c \
			utils5.c \
			utils6.c \
			utils7.c)

# objects
OBJS    = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# output name
NAME    = minishell

# default
all: $(LIBDIR)/libft.a $(NAME)

# link
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(EXTRAS)

# compile
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# obj directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# build libft bonus
$(LIBDIR)/libft.a:
	$(MAKE) -C $(LIBDIR) bonus

# clean obj files & libft
clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	$(MAKE) -C $(LIBDIR) clean

# clean all
fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBDIR) fclean

# rebuild
re: fclean all

.PHONY: all clean fclean re


