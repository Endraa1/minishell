#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "exec.h"
#include "parse.h"
#include "shell.h"

int main(void) {
    shell_t sh = {.last_status = 0, .running = 1};
    int interactive = isatty(STDIN_FILENO);

    /* TODO(milestone 6): make Ctrl-C interrupt the running child, not the
     * shell. Look at sigaction(2) and what happens to handlers across exec. */

    char *line = NULL;
    size_t cap = 0;
    while (sh.running) {
        if (interactive) {
            printf("minishell$ ");
            fflush(stdout);
        }
        if (getline(&line, &cap, stdin) == -1) {
            break; /* EOF (Ctrl-D) */
        }

        command_t *cmd = parse_line(line);
        if (cmd == NULL) {
            continue; /* empty line or syntax error */
        }
        fflush(stdout); /* don't let children inherit our buffered output */
        sh.last_status = execute(&sh, cmd);
        free_command(cmd);
    }

    free(line);
    if (interactive && sh.running) {
        printf("\n");
    }
    return sh.last_status;
}
