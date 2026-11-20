#include "builtins.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Worked example: use it as the pattern for the others. */
static int builtin_pwd(shell_t *sh, char **argv) {
    (void)sh;
    (void)argv;
    char buf[PATH_MAX];
    if (getcwd(buf, sizeof buf) == NULL) {
        perror("pwd");
        return 1;
    }
    printf("%s\n", buf);
    return 0;
}

static int builtin_cd(shell_t *sh, char **argv) {
    /* TODO(milestone 3): `cd dir` changes directory with chdir(2).
     * `cd` with no argument goes to $HOME (getenv). Print an error with
     * perror and return 1 if chdir fails. */
    (void)sh;
    (void)argv;
    fprintf(stderr, "cd: not implemented yet\n");
    return 1;
}

static int builtin_exit(shell_t *sh, char **argv) {
    /* TODO(milestone 3): stop the REPL by setting sh->running = 0.
     * `exit` returns sh->last_status; `exit N` returns N. */
    (void)sh;
    (void)argv;
    fprintf(stderr, "exit: not implemented yet\n");
    return 1;
}

static const struct {
    const char *name;
    builtin_fn fn;
} builtins[] = {
    {"pwd", builtin_pwd},
    {"cd", builtin_cd},
    {"exit", builtin_exit},
};

builtin_fn find_builtin(const char *name) {
    for (size_t i = 0; i < sizeof builtins / sizeof builtins[0]; i++) {
        if (strcmp(builtins[i].name, name) == 0) {
            return builtins[i].fn;
        }
    }
    return NULL;
}
