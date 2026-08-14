#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "shell.h"

/* Runs one external command using fork(), execvp(), and waitpid(). */
void execute_external(Command *cmd);

/* Redirects the calling process's stdout to filename using open()+dup2(). */
int redirect_stdout_to_file(const char *filename);

#endif
