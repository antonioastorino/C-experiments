//  Created by Antonio Astorino on 27/03/2021.

#include "fs_utils.h"
#include "result.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST 1

int main(int argc, char** argv) {
    if (argc != 1) { fprintf(stderr, "Parameters not supported"); }
    printf("Hello from `main`\n");
    printf("You are running this program from `%s`.\n", argv[0]);

#if TEST
#include "../test/common.c"
//#include "../test/test_class_string.c"
#include "../test/test_fs_utils.c"
#else
    printf("Hello from `main`\n");
    printf("You are running this program from `%s`.\n", argv[0]);
    printf("Run 'bin/build-run.sh test' to run the unit tests\n");
    // insert code here...
    FILE* file_1 = fopen("/Volumes/DataMBP/deleteme/file1.txt", "w+");
    if (file_1 == NULL) { fprintf(stderr, "Failed to open file: errno %d\n", errno); }

#endif
    return 0;
}
