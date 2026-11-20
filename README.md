# minishell

A small Unix shell written from scratch in C11: it reads commands, parses them, and runs programs with `fork`/`exec`. It supports pipes, I/O redirection and builtins.

![CI](https://github.com/Endraa1/minishell/actions/workflows/ci.yml/badge.svg)

<!-- Once it works: record a demo with `asciinema` or a GIF and put it here. -->

## Features

- [ ] Tokenizer with `'single'` and `"double"` quotes
- [ ] Run programs found on `$PATH` (`fork` + `execvp` + `waitpid`)
- [x] Builtin `pwd`
- [ ] Builtins `cd` and `exit [N]`
- [ ] Redirection: `<`, `>`, `>>`
- [ ] Pipelines of any length: `a | b | c`
- [ ] Ctrl-C interrupts the running program, not the shell
- [ ] Stretch: background jobs `&`, `$VAR` expansion, history

## Build & run

```bash
make
./minishell
```

```text
minishell$ printf 'banana\napple\ncherry\n' | sort | head -n 2 > fruit.txt
minishell$ cat < fruit.txt
apple
banana
```

## Testing

```bash
make test        # unit tests (parser) + end-to-end tests (feeds scripts to the shell)
make test-asan   # same, built with AddressSanitizer + UndefinedBehaviorSanitizer
make memcheck    # parser tests under valgrind
```

Tests are grouped by milestone (see [ROADMAP.md](ROADMAP.md)). `MILESTONE` in the `Makefile` sets which groups run. CI runs everything with gcc and clang, plus the sanitizers and valgrind.

## Design

```
main.c      read a line  →  parse.c  →  command_t list  →  exec.c  →  fork/exec/wait
                                                              └→ builtins.c (cd, exit, pwd)
```

- **`parse.c`**: turns a line into tokens, then into a linked list of `command_t` (one per pipeline stage), each with `argv`, `infile`, `outfile`.
- **`exec.c`**: forks one child per stage, wires the pipes with `pipe` + `dup2`, applies redirections, then `execvp`.
- **`builtins.c`**: commands that have to run inside the shell process itself (`cd` changes the *shell's* directory, `exit` stops the *shell*).

<!-- Add your own notes here as you build: bugs you hit and decisions you made.
     Interviewers love "the shell hung because I forgot to close a pipe fd". -->
