#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
OUT=$(mktemp)
trap 'rm -f "$OUT"' EXIT
cc -std=c11 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -fno-omit-frame-pointer -I"$ROOT/app/include" "$ROOT/app/src/ride_buttons.c" "$ROOT/tests/button_engine/test_buttons.c" -o "$OUT"
"$OUT"
