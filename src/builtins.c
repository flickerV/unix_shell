#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "../include/builtins.h"

static const char *BUILTIN_NAMES[] = {
    "cd", "pwd", "echo", "exit", "help",
    "history", "clear", "about", "system", "fortune", "stats"
};
static const int BUILTIN_COUNT = sizeof(BUILTIN_NAMES) / sizeof(BUILTIN_NAMES[0]);

int is_builtin(const char *argv0) {
    for (int i = 0; i < BUILTIN_COUNT; i++) {
        if (strcmp(argv0, BUILTIN_NAMES[i]) == 0) return 1;
    }
    return 0;
}

static void builtin_cd(Command *cmd) {
    const char *target = NULL;

    if (cmd->argc < 2 || strcmp(cmd->argv[1], "~") == 0) {
        target = getenv("HOME");
        if (target == NULL) target = "/";
    } else {
        target = cmd->argv[1];
    }

    if (chdir(target) != 0) {
        perror("flickerV: cd");
    }
}

static void builtin_pwd(void) {
    char cwd[MAX_LINE];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("flickerV: pwd");
    }
}

static void builtin_echo(Command *cmd) {
    for (int i = 1; i < cmd->argc; i++) {
        printf("%s", cmd->argv[i]);
        if (i != cmd->argc - 1) printf(" ");
    }
    printf("\n");
}

static void builtin_help(void) {
    printf(
        "\nflickerV - built-in commands\n"
        "  cd [dir]      change directory (also: cd ~)\n"
        "  pwd           print current directory\n"
        "  echo [text]   print text\n"
        "  history       show commands from this session\n"
        "  stats         show session statistics\n"
        "  system        show shell/user/OS information\n"
        "  fortune       print a small random quote\n"
        "  clear         clear the terminal screen\n"
        "  about         show how flickerV is built\n"
        "  help          show this screen\n"
        "  exit          leave flickerV\n"
        "\nshell features\n"
        "  external commands   e.g. ls, cat, grep\n"
        "  output redirection  cmd > file\n\n"
    );
}

static void builtin_about(void) {
    printf(
        "\nflickerV - Interactive Unix Shell\n\n"
        "Built in C for Linux/WSL.\n\n"
        "Core concepts:\n"
        "  - Processes\n"
        "  - fork()\n"
        "  - execvp()\n"
        "  - waitpid()\n"
        "  - File descriptors\n"
        "  - I/O redirection\n"
        "  - Signals\n"
        "  - Basic command parsing\n\n"
        "Built with: C | Linux | POSIX\n\n"
    );
}

static void builtin_history(void) {
    printf("\n");
    for (int i = 0; i < g_history_count; i++) {
        printf("%3d  %s\n", i + 1, g_history[i]);
    }
    printf("\n");
}

static void builtin_clear(void) {
    printf("\x1b[H\x1b[2J");
    fflush(stdout);
}

static void builtin_stats(void) {
    printf("\n");
    printf("+------------- Session Stats -------------+\n");
    printf("| Commands       %-3d                      |\n", g_stats.total_commands);
    printf("| Built-ins      %-3d                      |\n", g_stats.builtin_commands);
    printf("| External       %-3d                      |\n", g_stats.external_commands);
    printf("| Redirections   %-3d                      |\n", g_stats.redirections);
    printf("+------------------------------------------+\n\n");
}

static void builtin_system(void) {
    char cwd[MAX_LINE] = "?";
    getcwd(cwd, sizeof(cwd));

    struct utsname sys_info;
    if (uname(&sys_info) != 0) {
        perror("flickerV: system");
        return;
    }

    printf("\n");
    printf("Shell:      flickerV\n");
    printf("User:       %s\n", g_username);
    printf("PID:        %d\n", getpid());
    printf("Directory:  %s\n", cwd);
    printf("OS:         %s %s\n\n", sys_info.sysname, sys_info.release);
}

static const char *FORTUNES[] = {
    "Processes are just programs with a pulse.",
    "Keep calm and check your file descriptors.",
    "Every segmentation fault is a learning opportunity.",
    "Ctrl+C is just a polite way of saying stop.",
    "Every zombie process was once somebody's child.",
};
static const int FORTUNE_COUNT = sizeof(FORTUNES) / sizeof(FORTUNES[0]);

static void builtin_fortune(void) {
    printf("\n\"%s\"\n\n", FORTUNES[rand() % FORTUNE_COUNT]);
}

int run_builtin(Command *cmd) {
    const char *name = cmd->argv[0];

    if (strcmp(name, "cd") == 0)      { builtin_cd(cmd); return 0; }
    if (strcmp(name, "pwd") == 0)     { builtin_pwd(); return 0; }
    if (strcmp(name, "echo") == 0)    { builtin_echo(cmd); return 0; }
    if (strcmp(name, "help") == 0)    { builtin_help(); return 0; }
    if (strcmp(name, "about") == 0)   { builtin_about(); return 0; }
    if (strcmp(name, "history") == 0) { builtin_history(); return 0; }
    if (strcmp(name, "clear") == 0)   { builtin_clear(); return 0; }
    if (strcmp(name, "stats") == 0)   { builtin_stats(); return 0; }
    if (strcmp(name, "system") == 0)  { builtin_system(); return 0; }
    if (strcmp(name, "fortune") == 0) { builtin_fortune(); return 0; }
    if (strcmp(name, "exit") == 0)    { return 1; }

    return 0;
}
