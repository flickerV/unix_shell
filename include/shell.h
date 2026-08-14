#ifndef SHELL_H
#define SHELL_H

#include <time.h>

#define MAX_LINE     512
#define MAX_ARGS     32
#define MAX_HISTORY  200
#define MAX_NAME     64

typedef struct {
    char *argv[MAX_ARGS + 1];
    int   argc;
    char *outfile;
} Command;

typedef struct {
    int total_commands;
    int builtin_commands;
    int external_commands;
    int redirections;
} SessionStats;

extern char         g_username[MAX_NAME];
extern SessionStats g_stats;
extern char         g_history[MAX_HISTORY][MAX_LINE];
extern int          g_history_count;

void run_shell(void);
void print_welcome_and_ask_name(void);

#endif
