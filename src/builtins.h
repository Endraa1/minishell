#ifndef MINISHELL_BUILTINS_H
#define MINISHELL_BUILTINS_H

#include "shell.h"

/* A builtin gets the shell state and the command's argv; returns its exit
 * status. Builtins run inside the shell process (no fork), which is why
 * `cd` and `exit` must be builtins. */
typedef int (*builtin_fn)(shell_t *sh, char **argv);

/* Returns the builtin called `name`, or NULL if there isn't one. */
builtin_fn find_builtin(const char *name);

#endif
