#!/bin/zsh
BASE_DIR=$(dirname $0)
pushd "${BASE_DIR}/.." >/dev/null
/bin/rm -r test/artifacts/*

if [ "$1" = "test" ]; then
    # Set up dir entries for testing.
    mkdir -p test/artifacts/empty/ \
        test/artifacts/non-empty/inner/inner_l2 \
        test/artifacts/non-empty-0/inner/inner_l2 \
        test/artifacts/empty-0

    touch test/artifacts/non-empty/inner/file.txt
    touch test/artifacts/non-empty/inner/inner_l2/file.txt
    touch test/artifacts/deleteme.txt

    sed -i.bak 's/^#define TEST 0/#define TEST 1/' src/main.c
else
    sed -i.bak 's/^#define TEST 1/#define TEST 0/' src/main.c
fi
/bin/rm -f *.bak
clear; make && ./prj-out-0
popd >/dev/null
