#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "parser.h"
# include <readline/readline.h>

typedef struct s_var {
    char *name;
    char *value;
    bool exported;
} t_var;

typedef struct s_executor_ctx {
    int last_exit_status;
} t_executor_ctx;

void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx);
void    sigint_handler(int signo);
void    setup_signal_handlers(void);
//this a placeholder for my future executor lol so i dont contaminate yours.


#endif
