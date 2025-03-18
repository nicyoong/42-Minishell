/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tching <tching@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 22:19:31 by tching            #+#    #+#             */
/*   Updated: 2025/03/18 22:46:06 by tching           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <error.h>
# include <stdbool.h>
# include <fcntl.h>
# include <limits.h>
# include <signal.h>
# include <termios.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <dirent.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <curses.h>

//DEFINES
# define MEM_FAIL	"Memory allocation failed."
# define PIPE_FAIL	"The pipe() failed."
# define FORK_FAIL	"The fork() failed."
# define CLOSE_FAIL	"The close() failed."
# define DUP_FAIL	"The dup() failed."
# define DUP2_FAIL	"The dup2() failed."
# define EXECVE_FAIL	"The execve() failed."


//TERMINAL





//SHELL


//EXPANSION


//EXECUTION


//SIGNALS


//ERROR HANDLING


//FREE


//EXIT





#endif
