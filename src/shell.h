#ifndef MINISHELL_SHELL_H
#define MINISHELL_SHELL_H

/* State that lives for the whole shell session. Passed around instead of
 * using globals so every function's inputs are explicit. */
typedef struct shell {
    int last_status; /* exit status of the most recent command */
    int running;     /* set to 0 to leave the REPL (e.g. the `exit` builtin) */
} shell_t;

#endif
