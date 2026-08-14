# flickerV — Interactive Unix Shell

## What is flickerV?

flickerV is a small interactive Unix shell written in C for Linux/WSL. It reads commands, runs built-in commands itself, and launches external programs as separate Unix processes.

## Features

**Built-ins:** `cd`, `pwd`, `echo`, `exit`, `help`, `history`, `clear`, `about`, `system`, `fortune`, `stats`

**Unix mechanics:**
- External command execution with `fork()` + `execvp()` + `waitpid()`
- Output redirection with `command > file`
- Basic Ctrl+C (`SIGINT`) handling
- Session-only command history and statistics
- Simple whitespace-based command parsing
- Personalized prompt and startup screen

## Architecture

```text
User types a command
        |
        v
   shell.c (REPL)
        |
        v
   parser.c
        |
   +----+----+
   |         |
builtin   external
   |         |
   v         v
builtins.c  executor.c
            fork()
              |
           execvp()
              |
           waitpid()
```

For output redirection:

```text
open("file") -> dup2(fd, STDOUT_FILENO) -> command output goes to file
```

## Project structure

```text
unix_shell/
├── src/
│   ├── main.c
│   ├── shell.c
│   ├── parser.c
│   ├── executor.c
│   └── builtins.c
├── include/
├── tests/
├── Makefile
├── README.md
└── .gitignore
```
- `main.c` — starts the shell
- `shell.c` — shell loop, prompt, and input handling
- `parser.c` — command parsing
- `executor.c` — external command execution and redirection
- `builtins.c` — built-in commands

## Core Unix concepts

- **Program vs process:** a program is executable code; a process is a running instance of a program.
- **PID:** the kernel gives each running process an ID.
- **fork():** creates a child process so the shell can remain alive while an external command runs.
- **execvp():** replaces the child process's program with the requested command and searches `PATH` for it.
- **waitpid():** makes the parent shell wait for its child to finish.
- **File descriptors:** 0 is stdin, 1 is stdout, and 2 is stderr.
- **dup2():** redirects one file descriptor to another resource, which is how `>` works.
- **Signals:** Ctrl+C sends `SIGINT`; the shell survives while a running child can be interrupted.
- **Parsing:** the shell turns a command line into an argument array for execution.

## Build

On Ubuntu/WSL with GCC installed:

```bash
git clone https://github.com/flickerV/unix_shell.git
cd unix_shell
make
make run
```

To remove build artifacts:

```bash
make clean
```

## Example

```text
+========================================+
|              flickerV                  |
|         a tiny Unix shell              |
+========================================+

What's your name? Jane

Welcome, Jane!
Type 'help' to see what I can do.

flickerV@Jane ~/flickerV $ echo hello
hello

flickerV@Jane ~/flickerV $ echo hello > out.txt
flickerV@Jane ~/flickerV $ cat out.txt
hello

flickerV@Jane ~/flickerV $ fortune

"Processes are just programs with a pulse."

flickerV@Jane ~/flickerV $ exit

Goodbye, Jane! Come back soon.
```

