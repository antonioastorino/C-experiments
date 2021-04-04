#!/bin/zsh
BASE_DIR="$(dirname $0)/.."

LOG_FILE="${BASE_DIR}/test/test.log"
[ "$1" = "test" ] && MODE="test" || MODE="app"

pushd "${BASE_DIR}" >/dev/null
/bin/rm -r test/artifacts/*
/bin/rm "${LOG_FILE}"

if [ "${MODE}" = "test" ]; then
    make clean
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
echo -e "\n\n --- Building and running ${MODE} --- \n\n"
make && ./prj-out-0 2>"${LOG_FILE}"

if [ -f "${LOG_FILE}" ]; then
    if [ "$(cat ${LOG_FILE})" = "" ]; then
        echo -e "\n\n\e[32mSUCCESS:\e[0m - All tests passed.\n\n"
    else
        echo -e "\n\n\e[31mFAIL\e[0m - See ${LOG_FILE} for more info.\n\n"
    fi
else
    echo -e "\n\n\e[31mApplication not run.\e[0m\n\n"
fi
popd >/dev/null
