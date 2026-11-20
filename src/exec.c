#include "exec.h"

#include <stdio.h>

#include "builtins.h"

int execute(shell_t *sh, command_t *cmd) {
    /* A builtin on its own runs inside the shell process. */
    if (cmd->next == NULL) {
        builtin_fn fn = find_builtin(cmd->argv[0]);
        if (fn != NULL) {
            /* TODO(milestone 4): `pwd > file` should still redirect. */
            return fn(sh, cmd->argv);
        }
    }

    /* TODO(milestone 2): run a single external command.
     *   fork(); in the child call execvp(argv[0], argv); if it returns,
     *   print an error and _exit(127). In the parent, waitpid() and turn the
     *   status into an exit code (WIFEXITED / WEXITSTATUS).
     *
     * TODO(milestone 4): in the child, before execvp, open infile/outfile and
     *   dup2 them onto STDIN_FILENO / STDOUT_FILENO.
     *
     * TODO(milestone 5): for a pipeline of n commands, create n-1 pipes and
     *   fork n children, wiring each child's stdin/stdout to the right pipe
     *   ends. Close every pipe fd you don't need, in the children AND the
     *   parent, or readers will never see EOF and the shell hangs.
     *   Wait for all children; return the status of the last one.
     */
    fprintf(stderr, "minishell: %s: running programs not implemented yet\n",
            cmd->argv[0]);
    return 127;
}
