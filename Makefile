# Bump this as you finish each milestone in ROADMAP.md.
# `make test` only runs tests for milestones up to this one.
MILESTONE ?= 1

CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Werror -pedantic -g
CFLAGS  += -D_POSIX_C_SOURCE=200809L
LDFLAGS ?=

SRC  := src/parse.c src/exec.c src/builtins.c
OBJ  := $(SRC:src/%.c=build/%.o)
DEPS := $(OBJ:.o=.d) build/main.d

.PHONY: all test unit e2e debug test-asan memcheck format clean

SANFLAGS := -O0 -fsanitize=address,undefined -fno-omit-frame-pointer

all: minishell

minishell: build/main.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

build/test_parse: tests/test_parse.c $(OBJ) | build
	$(CC) $(CFLAGS) -Isrc -o $@ $^ $(LDFLAGS)

build:
	mkdir -p build

test: unit e2e

unit: build/test_parse
	./build/test_parse $(MILESTONE)

e2e: minishell
	MILESTONE=$(MILESTONE) ./tests/run_e2e.sh ./minishell

# Rebuild with AddressSanitizer + UBSan: catches out-of-bounds, use-after-free,
# leaks and undefined behaviour the moment they happen.
debug: clean
	$(MAKE) CFLAGS="$(CFLAGS) $(SANFLAGS)" LDFLAGS="$(SANFLAGS)"

# Same, then run every test against the sanitized build.
test-asan: clean
	$(MAKE) test CFLAGS="$(CFLAGS) $(SANFLAGS)" LDFLAGS="$(SANFLAGS)"

memcheck: build/test_parse
	valgrind --leak-check=full --error-exitcode=1 -q ./build/test_parse $(MILESTONE)

format:
	clang-format -i src/*.c src/*.h tests/*.c

clean:
	rm -rf build minishell

-include $(DEPS)
