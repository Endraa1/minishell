#ifndef MINISHELL_EXEC_H
#define MINISHELL_EXEC_H

#include "parse.h"
#include "shell.h"

/* Run a parsed pipeline. Returns the exit status of the last command in it,
 * which the caller stores in sh->last_status. */
int execute(shell_t *sh, command_t *cmd);

#endif
