#ifndef PARSER_H
#define PARSER_H

#include "shell.h"

/* Parses one simple command and optional > output redirection. */
int parse_line(char *line, Command *cmd);

#endif
