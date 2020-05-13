#!/usr/bin/env bash

set -e

if [ ! "$(uname -s)" = "Linux" ]; then
    exit 1
fi

target="$WD/bin/main_valgrind"

sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
perf record -g "$target"
perf report
rm perf.data*

if [ -z "$1" ]; then
    exit 0
fi

valgrind --tool=cachegrind "$target"
rm cachegrind.out.*
