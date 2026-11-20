# Roadmap

Do these in order. After each one:

1. Bump `MILESTONE` in the `Makefile`.
2. Run `make test` and `make test-asan` until both are green.
3. Commit with a real message (`Add pipeline support`, not `stuff`) and push.
4. Tick the box in the README.

Small, frequent commits look far better on GitHub than one giant dump.

Useful man pages: `man 2 fork`, `man 3 execvp`, `man 2 waitpid`, `man 2 pipe`, `man 2 dup2`, `man 2 open`, `man 2 sigaction`.

---

## Milestone 1: Parsing (`src/parse.c`)

Implement `tokenize()`, then the simple version of `parse_line()`.

- Write `tokenize()` as a loop over the characters. At each position you're either skipping whitespace, emitting an operator (`|`, `<`, `>`, `>>`), or building a word. While you build a word, a quote switches you into "copy until the matching quote" mode.
- Build each word in a buffer of `strlen(line) + 1` bytes. A token can never be longer than the line.
- `parse_line()` for now: tokenize, `calloc` one `command_t`, copy (`strdup`) every token into `argv`, and NULL-terminate it.
- ✅ `pass tokenize`, `pass parse_simple`, `pass builtin pwd`

## Milestone 2: Running programs (`src/exec.c`)

- `fork()` returns twice. Child: `execvp(argv[0], argv)`. If that returns at all, it failed, so print an error and `_exit(127)`. Parent: `waitpid(pid, &status, 0)`.
- Convert `status` with `WIFEXITED`/`WEXITSTATUS`.
- Why `_exit` and not `exit` in the child? Find out: it involves stdio buffers.
- ✅ `pass echo`, `pass quoted args`, `pass unknown command`

## Milestone 3: Builtins (`src/builtins.c`)

- Think about why `cd` can't be an external program like `/bin/cd`.
- `exit` stops the loop in `main.c` through `sh->running`.
- ✅ `pass cd then pwd`, `pass exit status`

## Milestone 4: Redirection

- Parser: when you see `<`, `>` or `>>`, the *next* token is a filename, not an argument. A redirect with no filename is a syntax error.
- Exec: in the child, after `fork` and before `execvp`: `open` the file, `dup2(fd, STDOUT_FILENO)`, `close(fd)`.
- Flags: `>` is `O_WRONLY|O_CREAT|O_TRUNC`, `>>` is `O_WRONLY|O_CREAT|O_APPEND`, and use mode `0644`.
- ✅ `pass redirect out`, `pass append`, `pass redirect in`

## Milestone 5: Pipelines

- Parser: `|` starts a new `command_t`. Empty stages (`ls |`, `| ls`, `ls || wc`) are syntax errors.
- Exec: you need one `pipe()` between each pair of commands. Child *i* reads from pipe *i-1* and writes to pipe *i*.
- **The classic bug:** if any process (including the parent) still has a pipe's write end open, the reader never gets EOF and the shell hangs. Close everything you aren't using.
- Debug with `ls -l /proc/<pid>/fd` from another terminal.
- ✅ every e2e test passes

## Milestone 6: Signals

- Pressing Ctrl-C at the prompt shouldn't kill your shell, but it *should* kill `sleep 100`.
- Hint: the shell ignores `SIGINT`; children restore the default handler before `execvp`. (Handlers that are *ignored* survive exec; handlers set to a function don't.)
- No automated test here. Check it by hand, and describe how in the README.

## Stretch goals (pick any)

- `$VAR` and `$?` expansion
- `cmd &` background jobs, and reaping zombies with `SIGCHLD`
- Command history with GNU readline (`-lreadline`)
- `;` and `&&` / `||`
- Write your own e2e tests in `tests/run_e2e.sh` for each one
