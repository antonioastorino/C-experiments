#!/bin/zsh
BASE_DIR=`dirname $0`
pushd "${BASE_DIR}/.." > /dev/null 
/bin/rm -r test/artifacts/*

if [ "$1" = "test" ]; then
    sed -i.bak 's/^#define TEST 0/#define TEST 1/' src/main.c
else
    sed -i.bak 's/^#define TEST 1/#define TEST 0/' src/main.c
fi
/bin/rm -f *.bak
make && clear && ./prj-out-0
popd >/dev/null
