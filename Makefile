# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tiara <tching@student.42kl.edu.my>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/18 18:33:55 by tching            #+#    #+#              #
#    Updated: 2025/05/18 18:34:48 by tiara            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


CC      = gcc
AR      = ar
CFLAGS  = -Wall -Wextra -Werror -Iinc -Ilibft
LDFLAGS = -Llibft -lft
EXTRAS	= -lreadline -no-pie

SRCDIR  = src
OBJDIR  = obj
LIBDIR  = libft
INCDIR  = inc

SRCS    = $(wildcard $(SRCDIR)/*.c)
OBJS    = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
NAME    = minishell

all: $(LIBDIR)/libft.a $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(EXTRAS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(LIBDIR)/libft.a:
	$(MAKE) -C $(LIBDIR) bonus

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	$(MAKE) -C $(LIBDIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBDIR) fclean

re: fclean all

.PHONY: all clean fclean re