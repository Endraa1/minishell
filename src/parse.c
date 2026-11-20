#include "parse.h"

#include <stdlib.h>

/* ---- Milestone 1 ------------------------------------------------------ */

char **tokenize(const char *line, int *ntokens) {
    /* TODO(milestone 1): implement the rules described in parse.h.
     * Hint: walk the string with an index, and grow a dynamic array of
     * tokens (start with capacity 8, double with realloc when full). */
    (void)line;
    *ntokens = 0;
    return NULL;
}

void free_tokens(char **tokens) {
    if (tokens == NULL) {
        return;
    }
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

/* ---- Milestones 1, 4, 5 ----------------------------------------------- */

command_t *parse_line(const char *line) {
    /* TODO(milestone 1): tokenize, then build a single command_t whose argv
     *                    holds every token.
     * TODO(milestone 4): treat `<`, `>`, `>>` + the following token as
     *                    redirections instead of arguments.
     * TODO(milestone 5): start a new command_t at every `|`.
     * Remember: return NULL for empty lines and syntax errors, and don't leak
     * memory on the error paths. */
    (void)line;
    return NULL;
}

void free_command(command_t *cmd) {
    while (cmd != NULL) {
        command_t *next = cmd->next;
        for (int i = 0; i < cmd->argc; i++) {
            free(cmd->argv[i]);
        }
        free(cmd->argv);
        free(cmd->infile);
        free(cmd->outfile);
        free(cmd);
        cmd = next;
    }
}
