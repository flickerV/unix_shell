#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include "../include/shell.h"
#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/builtins.h"

char         g_username[MAX_NAME] = "friend";
SessionStats g_stats;
char         g_history[MAX_HISTORY][MAX_LINE];
int          g_history_count = 0;

static void sigint_handler(int signo) {
    (void)signo;
    write(STDOUT_FILENO, "\n", 1);
}

static void install_sigint_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}

static void print_welcome_banner(void) {
    printf("\n");
    printf("+========================================+\n");
    printf("|              flickerV                |\n");
    printf("|         a tiny Unix shell              |\n");
    printf("+========================================+\n\n");
}

static void ask_username(void) {
    printf("What's your name? ");
    fflush(stdout);

    char buf[MAX_NAME];
    if (fgets(buf, sizeof(buf), stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
        if (buf[0] != '\0') {
            strncpy(g_username, buf, MAX_NAME - 1);
            g_username[MAX_NAME - 1] = '\0';
        }
    }

    printf("\nWelcome, %s!\n", g_username);
    printf("Type 'help' to see what I can do.\n\n");
}

static void print_prompt(void) {
    char cwd[MAX_LINE];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "?");
    }

    const char *home = getenv("HOME");
    const char *display = cwd;
    char shortened[MAX_LINE];

    if (home != NULL && strncmp(cwd, home, strlen(home)) == 0) {
        snprintf(shortened, sizeof(shortened), "~%s", cwd + strlen(home));
        display = shortened;
    }

    printf("flickerV@%s %s $ ", g_username, display);
    fflush(stdout);
}

static int is_blank(const char *s) {
    while (*s) {
        if (!isspace((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

/* Builtins normally run in the shell process. If stdout must be redirected,
 * use a temporary child so the shell's own stdout is not permanently changed. */
static int run_builtin_maybe_redirected(Command *cmd) {
    if (cmd->outfile == NULL) {
        return run_builtin(cmd);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 0;
    }

    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        if (redirect_stdout_to_file(cmd->outfile) != 0) exit(1);
        run_builtin(cmd);
        exit(0);
    }

    waitpid(pid, NULL, 0);
    return 0;
}

void run_shell(void) {
    char raw_line[MAX_LINE];
    char parse_buf[MAX_LINE];

    srand((unsigned)time(NULL));
    install_sigint_handler();

    for (;;) {
        print_prompt();

        if (fgets(raw_line, sizeof(raw_line), stdin) == NULL) {
            if (errno == EINTR) {
                clearerr(stdin);
                continue;
            }
            printf("\n");
            break;
        }

        raw_line[strcspn(raw_line, "\n")] = '\0';
        if (is_blank(raw_line)) continue;

        if (g_history_count < MAX_HISTORY) {
            strncpy(g_history[g_history_count], raw_line, MAX_LINE - 1);
            g_history[g_history_count][MAX_LINE - 1] = '\0';
            g_history_count++;
        }

        strncpy(parse_buf, raw_line, MAX_LINE - 1);
        parse_buf[MAX_LINE - 1] = '\0';

        Command cmd;
        if (parse_line(parse_buf, &cmd) != 0) continue;

        g_stats.total_commands++;
        if (cmd.outfile != NULL) g_stats.redirections++;

        if (is_builtin(cmd.argv[0])) {
            g_stats.builtin_commands++;
            if (run_builtin_maybe_redirected(&cmd)) break;
        } else {
            g_stats.external_commands++;
            execute_external(&cmd);
        }
    }

    printf("\nGoodbye, %s! Come back soon.\n", g_username);
}

void print_welcome_and_ask_name(void) {
    print_welcome_banner();
    ask_username();
}
