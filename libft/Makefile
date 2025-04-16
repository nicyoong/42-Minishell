# Variables
CC = cc
AR = ar
CFLAGS = -Wall -Wextra -Werror
SRCS = \
    ft_isalpha.c \
    ft_isdigit.c \
    ft_isalnum.c \
    ft_isascii.c \
    ft_isprint.c \
    ft_strlen.c \
    ft_memset.c \
    ft_bzero.c \
    ft_memcpy.c \
    ft_memmove.c \
    ft_strlcpy.c \
    ft_strlcat.c \
    ft_toupper.c \
    ft_tolower.c \
    ft_strchr.c \
    ft_strrchr.c \
    ft_strncmp.c \
    ft_memchr.c \
    ft_memcmp.c \
    ft_strnstr.c \
    ft_atoi.c \
    ft_calloc.c \
    ft_strdup.c \
    ft_substr.c \
    ft_strjoin.c \
    ft_strtrim.c \
    ft_split.c \
    ft_itoa.c \
    ft_strmapi.c \
    ft_striteri.c \
    ft_putchar_fd.c \
    ft_putstr_fd.c \
    ft_putendl_fd.c \
    ft_putnbr_fd.c \
    get_next_line.c \
    ft_printf.c \
    handler.c \
    handler2.c \
    utils.c

BONUS_SRCS = \
    ft_lstnew.c \
    ft_lstadd_front.c \
	ft_lstsize.c \
	ft_lstlast.c \
	ft_lstadd_back.c \
	ft_lstdelone.c \
	ft_lstclear.c \
	ft_lstiter.c \
	ft_lstmap.c

# Object files for normal and bonus sources
OBJS = $(SRCS:.c=.o)
BONUS_OBJS = $(BONUS_SRCS:.c=.o)

# Library name
NAME = libft.a
SO_NAME = libft.so

# Phony targets
.PHONY: all clean fclean re bonus so

# Default target
all: $(NAME)

# Create the static library (for normal compilation)
$(NAME): $(OBJS)
	$(AR) rcs $@ $^

# Compile source files into object files (including bonus if needed)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create the shared library (so target) including the bonus files
so: $(OBJS) $(BONUS_OBJS)
	# Recompile object files with -fPIC for shared library
	$(CC) $(CFLAGS) -fPIC -c $(SRCS) $(BONUS_SRCS)
	$(CC) -shared -o $(SO_NAME) $(OBJS) $(BONUS_OBJS)

# Bonus target (compile and add bonus functions to the static library)
bonus: $(OBJS) $(BONUS_OBJS)
	$(AR) rcs $(NAME) $(OBJS) $(BONUS_OBJS)

# Clean object files
clean:
	rm -f $(OBJS) $(BONUS_OBJS)

# Clean object files and the libraries
fclean: clean
	rm -f $(NAME) $(SO_NAME)

# Rebuild the library
re: fclean all
