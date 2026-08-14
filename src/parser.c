#include <stdio.h>
#include <string.h>
#include "../include/parser.h"

/*
 * flickerV intentionally has a tiny grammar:
 *
 *   command [args...]
 *   command [args...] > file
 *
 * There is no quoting, variable expansion, piping, or input redirection.
 */
int parse_line(char *line, Command *cmd) {
    cmd->argc = 0;
    cmd->outfile = NULL;

    char *tok = strtok(line, " \t");
    while (tok != NULL) {
        if (cmd->argc >= MAX_ARGS) {
            fprintf(stderr, "flickerV: too many arguments\n");
            return -1;
        }

        cmd->argv[cmd->argc++] = tok;
        tok = strtok(NULL, " \t");
    }

    cmd->argv[cmd->argc] = NULL;

    if (cmd->argc == 0) {
        fprintf(stderr, "flickerV: empty command\n");
        return -1;
    }

    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "|") == 0) {
            fprintf(stderr, "flickerV: pipes are not supported in this version\n");
            return -1;
        }
        if (strcmp(cmd->argv[i], "<") == 0) {
            fprintf(stderr, "flickerV: input redirection (<) is not supported\n");
            return -1;
        }
        if (strcmp(cmd->argv[i], ">>") == 0) {
            fprintf(stderr, "flickerV: append redirection (>>) is not supported; use >\n");
            return -1;
        }
    }

    /* Only a final " > filename" is accepted. */
    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], ">") == 0) {
            if (i == 0) {
                fprintf(stderr, "flickerV: syntax error, missing command before '>'\n");
                return -1;
            }
            if (i != cmd->argc - 2) {
                fprintf(stderr, "flickerV: syntax error near '>'\n");
                return -1;
            }

            cmd->outfile = cmd->argv[i + 1];
            cmd->argv[i] = NULL;
            cmd->argc = i;
            return 0;
        }
    }

    return 0;
}
