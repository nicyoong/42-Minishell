#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "parser.h"

typedef struct s_executor_ctx {
    int last_exit_status;
} t_executor_ctx;

void execute_pipeline(t_pipeline *pipeline, t_executor_ctx *ctx);
//this a placeholder for my future executor lol so i dont contaminate yours.


#endif
