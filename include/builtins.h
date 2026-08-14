#ifndef BUILTINS_H
#define BUILTINS_H

#include "shell.h"

int is_builtin(const char *argv0);
int run_builtin(Command *cmd);

#endif
