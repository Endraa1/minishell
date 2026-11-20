#ifndef MINISHELL_PARSE_H
#define MINISHELL_PARSE_H

/*
 * One simple command in a pipeline, e.g. for
 *
 *     grep foo < in.txt | sort >> out.txt
 *
 * you get two commands linked by `next`:
 *   [0] argv = {"grep", "foo", NULL}, infile = "in.txt"
 *   [1] argv = {"sort", NULL},        outfile = "out.txt", append = 1
 */
typedef struct command {
    char **argv;          /* NULL-terminated, ready for execvp */
    int argc;             /* number of entries in argv, not counting NULL */
    char *infile;         /* from `< file`, or NULL */
    char *outfile;        /* from `> file` or `>> file`, or NULL */
    int append;           /* 1 if outfile came from `>>` */
    struct command *next; /* next command in the pipeline, or NULL */
} command_t;

/*
 * Split a line into tokens.
 *
 * Rules:
 *   - whitespace separates tokens and is otherwise ignored
 *   - '...' and "..." group text (including spaces) into one token;
 *     the quote characters themselves are removed
 *   - |  <  >  >>  are always their own token, even with no spaces
 *     around them (`ls|wc` -> "ls", "|", "wc")
 *
 * Returns a malloc'd, NULL-terminated array of malloc'd strings and stores
 * the count in *ntokens. Returns NULL on error (e.g. an unterminated quote).
 * Free the result with free_tokens().
 */
char **tokenize(const char *line, int *ntokens);
void free_tokens(char **tokens);

/*
 * Turn a line into a linked list of commands (see command_t).
 * Returns NULL for an empty line or a syntax error (`ls |`, `> `, ...).
 * Free the result with free_command().
 */
command_t *parse_line(const char *line);
void free_command(command_t *cmd);

#endif
