# flickerV — Manual Test Plan

## External commands

```text
ls
ls -la
pwd
echo hello
cat file.txt
```

Expected: normal Unix output and a new prompt after each command.

## Built-ins

```text
cd ..
cd .
cd ~
pwd
echo hello world
history
help
about
system
stats
fortune
clear
exit
```

Expected: `cd` changes the directory shown in the prompt. `exit` ends the shell with the personalized goodbye message.

## Output redirection

```text
echo hello > test.txt
cat test.txt
ls > files.txt
cat files.txt
```

Expected: output is written to the specified file instead of the terminal.

## Unsupported features

```text
ls | grep txt
echo hi >> file.txt
cat < file.txt
>
```

Expected: a clear `flickerV:` message and a normal prompt. The shell must not crash.

## Errors

```text
doesnotexist
cd doesnotexist
```

Expected: a clear error and a working prompt afterward.

## Empty input

Press Enter several times.

Expected: the prompt simply returns. Empty lines are not added to history.

## Ctrl+C

Run:

```text
sleep 20
```

then press Ctrl+C.

Expected: `sleep` stops, but flickerV remains alive.

Press Ctrl+C at an empty prompt.

Expected: flickerV remains alive and shows a fresh prompt.

## Ctrl+D

Press Ctrl+D at the prompt.

Expected: flickerV exits cleanly with the goodbye message.

## Statistics

Run:

```text
pwd
ls
echo hi > o.txt
stats
```

Expected counts should reflect 3 commands before `stats`: 2 built-ins, 1 external command, and 1 redirection. The `stats` command itself increments the total command count before displaying the current statistics.
