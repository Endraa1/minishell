/*
 * Unit tests for the parser. No framework, just a few macros.
 *
 * Usage: ./build/test_parse [MILESTONE]
 * Only runs tests for milestones <= MILESTONE (default 1), so you can keep
 * CI green while you work through the roadmap.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

static int failures = 0;
static int checks = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        checks++;                                                            \
        if (!(cond)) {                                                       \
            failures++;                                                      \
            fprintf(stderr, "  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        }                                                                    \
    } while (0)

#define CHECK_STR(got, want)                                                 \
    do {                                                                     \
        checks++;                                                            \
        const char *g_ = (got), *w_ = (want);                                \
        if (g_ == NULL || w_ == NULL ? g_ != w_ : strcmp(g_, w_) != 0) {     \
            failures++;                                                      \
            fprintf(stderr, "  FAIL %s:%d: %s == \"%s\", expected \"%s\"\n", \
                    __FILE__, __LINE__, #got, g_ ? g_ : "(null)",            \
                    w_ ? w_ : "(null)");                                     \
        }                                                                    \
    } while (0)

/* Tokenize `line` and compare against a NULL-terminated list of expected
 * tokens. Pass want == NULL to expect tokenize() to fail. */
static void expect_tokens(const char *line, const char *const *want) {
    int n = -1;
    char **got = tokenize(line, &n);
    if (want == NULL) {
        CHECK(got == NULL);
        free_tokens(got);
        return;
    }
    CHECK(got != NULL);
    if (got == NULL) {
        fprintf(stderr, "       (tokenize(\"%s\") returned NULL)\n", line);
        return;
    }
    int want_n = 0;
    while (want[want_n] != NULL) {
        want_n++;
    }
    CHECK(n == want_n);
    for (int i = 0; i < want_n && i < n; i++) {
        CHECK_STR(got[i], want[i]);
    }
    if (n == want_n) {
        CHECK(got[n] == NULL);
    }
    free_tokens(got);
}

#define TOKENS(...) ((const char *const[]){__VA_ARGS__, NULL})

static void test_tokenize(void) {
    expect_tokens("ls", TOKENS("ls"));
    expect_tokens("ls -l /tmp\n", TOKENS("ls", "-l", "/tmp"));
    expect_tokens("   echo    hi   ", TOKENS("echo", "hi"));
    expect_tokens("echo \"hello world\"", TOKENS("echo", "hello world"));
    expect_tokens("echo 'a  b'", TOKENS("echo", "a  b"));
    expect_tokens("echo 'say \"hi\"'", TOKENS("echo", "say \"hi\""));
    expect_tokens("ls|wc -l", TOKENS("ls", "|", "wc", "-l"));
    expect_tokens("cat<in>out", TOKENS("cat", "<", "in", ">", "out"));
    expect_tokens("echo hi >>log", TOKENS("echo", "hi", ">>", "log"));
    expect_tokens("echo 'unterminated", NULL);

    int n = -1;
    char **empty = tokenize("   \n", &n);
    CHECK(empty != NULL && n == 0 && empty[0] == NULL);
    free_tokens(empty);
}

static void test_parse_simple(void) {
    command_t *c = parse_line("echo hello world\n");
    CHECK(c != NULL);
    if (c == NULL) return;
    CHECK(c->argc == 3);
    CHECK_STR(c->argv[0], "echo");
    CHECK_STR(c->argv[2], "world");
    CHECK(c->argv[3] == NULL);
    CHECK(c->infile == NULL && c->outfile == NULL && c->next == NULL);
    free_command(c);

    CHECK(parse_line("") == NULL);
    CHECK(parse_line("   \n") == NULL);
}

static void test_parse_redirect(void) {
    command_t *c = parse_line("sort < in.txt > out.txt");
    CHECK(c != NULL);
    if (c == NULL) return;
    CHECK(c->argc == 1);
    CHECK_STR(c->argv[0], "sort");
    CHECK_STR(c->infile, "in.txt");
    CHECK_STR(c->outfile, "out.txt");
    CHECK(c->append == 0);
    free_command(c);

    c = parse_line("echo hi >> log");
    CHECK(c != NULL && c->append == 1);
    if (c) CHECK_STR(c->outfile, "log");
    free_command(c);

    CHECK(parse_line("echo hi >") == NULL);
}

static void test_parse_pipeline(void) {
    command_t *c = parse_line("cat f | grep x | wc -l");
    CHECK(c != NULL);
    if (c == NULL) return;
    CHECK_STR(c->argv[0], "cat");
    CHECK(c->next != NULL);
    if (c->next) {
        CHECK_STR(c->next->argv[1], "x");
        CHECK(c->next->next != NULL);
        if (c->next->next) {
            CHECK_STR(c->next->next->argv[0], "wc");
            CHECK(c->next->next->next == NULL);
        }
    }
    free_command(c);

    CHECK(parse_line("ls |") == NULL);
    CHECK(parse_line("| ls") == NULL);
    CHECK(parse_line("ls || wc") == NULL);
}

static const struct {
    int milestone;
    const char *name;
    void (*fn)(void);
} tests[] = {
    {1, "tokenize", test_tokenize},
    {1, "parse_simple", test_parse_simple},
    {4, "parse_redirect", test_parse_redirect},
    {5, "parse_pipeline", test_parse_pipeline},
};

int main(int argc, char **argv) {
    int milestone = argc > 1 ? atoi(argv[1]) : 1;
    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i++) {
        if (tests[i].milestone > milestone) {
            printf("skip %s (milestone %d)\n", tests[i].name, tests[i].milestone);
            continue;
        }
        int before = failures;
        tests[i].fn();
        printf("%s %s\n", failures == before ? "pass" : "FAIL", tests[i].name);
    }
    printf("\n%d/%d checks passed\n", checks - failures, checks);
    return failures == 0 ? 0 : 1;
}
