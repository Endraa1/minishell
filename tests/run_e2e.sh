#!/usr/bin/env bash
# End-to-end tests: feed a script to the shell on stdin, compare stdout.
# Usage: MILESTONE=N tests/run_e2e.sh ./minishell
set -u

SHELL_BIN=${1:-./minishell}
MILESTONE=${MILESTONE:-1}
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

pass=0
fail=0

# check MILESTONE NAME INPUT EXPECTED_STDOUT [EXPECTED_EXIT_CODE]
check() {
    local m=$1 name=$2 input=$3 want=$4 want_code=${5:-}
    if (( m > MILESTONE )); then
        echo "skip $name (milestone $m)"
        return
    fi
    local got code
    got=$(cd "$TMP" && printf '%s\n' "$input" | timeout 5 "$SHELL_BIN" 2>/dev/null)
    code=$?
    if [[ "$got" == "$want" && ( -z "$want_code" || "$code" == "$want_code" ) ]]; then
        echo "pass $name"
        pass=$((pass + 1))
    else
        echo "FAIL $name"
        echo "     input:    $(printf '%q' "$input")"
        echo "     expected: $(printf '%q' "$want")${want_code:+ (exit $want_code)}"
        echo "     got:      $(printf '%q' "$got") (exit $code)"
        fail=$((fail + 1))
    fi
}

SHELL_BIN=$(cd "$(dirname "$SHELL_BIN")" && pwd)/$(basename "$SHELL_BIN")
REAL_TMP=$(cd "$TMP" && pwd -P)

# --- Milestone 1: parsing (pwd is already implemented as a builtin) ---
check 1 "builtin pwd"          "pwd"                      "$REAL_TMP"
check 1 "blank lines ignored"  $'\n   \npwd'              "$REAL_TMP"

# --- Milestone 2: running programs ---
check 2 "echo"                 "echo hello"               "hello"
check 2 "quoted args"          "echo 'a   b' \"c d\""     "a   b c d"
check 2 "two commands"         $'echo one\necho two'      $'one\ntwo'
check 2 "unknown command"      $'nosuchcmd_xyz\necho ok'  "ok"

# --- Milestone 3: builtins ---
check 3 "cd then pwd"          $'cd /\npwd'               "/"
check 3 "exit stops the shell" $'exit\necho nope'         ""
check 3 "exit status"          "exit 3"                   ""     3

# --- Milestone 4: redirection ---
check 4 "redirect out"         $'echo hi > f\ncat f'      "hi"
check 4 "append"               $'echo a > f\necho b >> f\ncat f' $'a\nb'
check 4 "redirect in"          $'echo zzz > f\ncat < f'   "zzz"

# --- Milestone 5: pipes ---
check 5 "single pipe"          "echo hello | tr a-z A-Z"  "HELLO"
check 5 "long pipeline"        "printf 'c\nb\na\n' | sort | head -n 1" "a"
check 5 "pipe + redirect"      $'echo x | cat > f\ncat f' "x"

echo
echo "$pass passed, $fail failed (milestone $MILESTONE)"
(( fail == 0 ))
