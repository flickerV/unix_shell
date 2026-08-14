#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "../include/executor.h"

int redirect_stdout_to_file(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror(filename);
        return -1;
    }

    /* Make stdout (fd 1) point to the opened file. */
    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror("dup2");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

void execute_external(Command *cmd) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        /* The shell handles Ctrl+C itself; commands should use the
         * normal SIGINT behaviour so Ctrl+C can stop them. */
        signal(SIGINT, SIG_DFL);

        if (cmd->outfile != NULL && redirect_stdout_to_file(cmd->outfile) != 0) {
            exit(1);
        }

        /* execvp() replaces this child with the requested program. */
        execvp(cmd->argv[0], cmd->argv);
        fprintf(stderr, "flickerV: %s: command not found\n", cmd->argv[0]);
        exit(127);
    }

    /* The shell waits while the external command runs. */
    waitpid(pid, NULL, 0);
}
