#!/bin/bash
APP_NAME="app_name"
COMMON_HEADER="include/common.h"
CFLAGS="-Wall -Wextra -g"
GLOBAL_COMPILER="clang"
LIB="-lc++"
BUILD_DIR="build"
EXECUTABLE="${BUILD_DIR}/${APP_NAME}"
MAKE_FILE="Makefile"
MAIN="main"
MAIN_TEST="main-test"
SRC_EXTENSIONS=("c")
INC_EXTENSIONS=("h")

HEADER_PATHS="include"
SRC_PATHS="src"

ARTIFACT_FOLDER="test/artifacts"

LOG_FILE_ERR="test/test-err.log"
