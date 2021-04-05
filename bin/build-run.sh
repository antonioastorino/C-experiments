#!/bin/zsh
BASE_DIR="$(dirname $0)/.."

LOG_FILE="${BASE_DIR}/test/test.log"
if [ "$1" = "test" ]; then
    MODE="test"
elif [ "$1" = "debug" ]; then
    MODE="debug"
else
    MODE="app"
fi

pushd "${BASE_DIR}" >/dev/null
/bin/rm -r test/artifacts/*
/bin/rm "${LOG_FILE}"

if [ "${MODE}" = "test" ] || [ "${MODE}" = "debug" ]; then
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
if [ "${MODE}" = "test" ]; then
    make && ./prj-out-0 2>"${LOG_FILE}"
    RET_VAL=$?
    if [ ${RET_VAL} -ne 0 ]; then
        echo -e "\n\n\e[31mFAIL\e[0m - Execution interrupted with error code ${RET_VAL}.\n\n"
        exit ${RET_VAL}
    fi

    if [ -f "${LOG_FILE}" ]; then
        if [ "$(cat ${LOG_FILE})" = "" ]; then
            echo -e "\n\n\e[32mSUCCESS:\e[0m - All tests passed.\n\n"
        else
            echo -e "\n\n\e[31mFAIL\e[0m - The content of ${LOG_FILE} follows.\n\n"
            cat "${LOG_FILE}"
        fi
    else
        echo -e "\n\n\e[31mApplication not run.\e[0m\n\n"
    fi
elif [ "${MODE}" = "debug" ]; then
    make && lldb ./prj-out-0
else
    make && ./prj-out-0
fi
popd >/dev/null
