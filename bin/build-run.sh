#!/bin/zsh
BASE_DIR="$(dirname $0)/.."

LOG_FILE_ERR="${BASE_DIR}/test/test-err.log"
LOG_FILE_OUT="${BASE_DIR}/test/test-out.log"
if [ "$1" = "test" ]; then
    MODE="test"
elif [ "$1" = "debug" ]; then
    MODE="debug"
else
    MODE="app"
fi

analyze_mem() {
    while read -r line; do
        if [ "$(echo $line | grep -c '_PTR')" -eq 1 ]; then
            PTR=$(echo $line | awk '{print $2}')
            if [ $(cat $1 | grep -c "ALLOC_PTR: ${PTR}") -ne $(cat $1 | grep -c "FREE_PTR: ${PTR}") ]; then
                echo "WARNING: memory leaking for ${PTR}"
            fi
        fi
    done <$1
}

pushd "${BASE_DIR}" >/dev/null
/bin/rm -r test/artifacts/*
/bin/rm "${LOG_FILE_ERR}"

for file in $(find . -name "*.c"); do clang-format -i ${file}; done
for file in $(find . -name "*.h"); do clang-format -i ${file}; done

if [ "${MODE}" = "test" ] || [ "${MODE}" = "debug" ]; then
    make clean
    # Set up dir entries for testing.
    mkdir -p test/artifacts/empty/ \
        test/artifacts/non-empty/inner/inner_l2 \
        test/artifacts/non-empty-0/inner/inner_l2 \
        test/artifacts/empty-0

    touch test/artifacts/non-empty/inner/file.txt
    touch test/artifacts/non-empty/inner/inner_l2/file.txt
    touch test/artifacts/delete_me.txt

    sed -i.bak 's/^#define TEST 0/#define TEST 1/' "src/main.c"
    sed -i.bak 's/^#define MEM_ANALYSIS 0/#define MEM_ANALISYS 1/' "include/logger.h"
    sed -i.bak 's/^#define LOG_LEVEL 0/#define LOG_LEVEL 1/' "include/logger.h"

else
    sed -i.bak 's/^#define TEST 1/#define TEST 0/' "src/main.c"
    sed -i.bak 's/^#define MEM_ANALYSIS 1/#define MEM_ANALISYS 0/' "include/logger.h"
    sed -i.bak 's/^#define LOG_LEVEL 1/#define LOG_LEVEL 0/' "include/logger.h"
fi
/bin/rm -f *.bak
echo -e "\n\n --- Building and running ${MODE} --- \n\n"
if [ "${MODE}" = "test" ]; then
    make && ./prj-out-0 1>"${LOG_FILE_OUT}" 2>"${LOG_FILE_ERR}"
    RET_VAL=$?
    if [ ${RET_VAL} -ne 0 ]; then
        echo -e "\n\n\e[31mFAIL\e[0m - Execution interrupted with error code ${RET_VAL}.\n\n"
        exit ${RET_VAL}
    fi

    if [ -f "${LOG_FILE_ERR}" ]; then
        if [ "$(cat ${LOG_FILE_ERR})" = "" ]; then
            echo -e "\n\n\e[32mSUCCESS:\e[0m - All tests passed.\n\n"
        else
            echo -e "\n\n\e[31mFAIL\e[0m - The content of ${LOG_FILE_ERR} follows.\n\n"
            cat "${LOG_FILE_ERR}"
        fi
    else
        echo -e "\n\n\e[31mApplication not run.\e[0m\n\n"
    fi
    analyze_mem "${LOG_FILE_OUT}"
elif [ "${MODE}" = "debug" ]; then
    make && lldb ./prj-out-0
else
    make && ./prj-out-0
fi
popd >/dev/null
