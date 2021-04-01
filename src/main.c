//  Created by Antonio Astorino on 27/03/2021.

#include "fs_utils.h"
#include "result.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST 0

int main() {
#if TEST
#include "../test/fs_utils_test.c"
#include "../test/string_utils_test.c"
#else
    printf("Hello from `main`\n");
    printf("Run 'bin/build-run.sh test' to run the unit tests\n");
    // insert code here...
    FILE* file_1 = fopen("/Volumes/DataMBP/deleteme/file1.txt", "w+");
    if (file_1 == NULL) { fprintf(stderr, "Failed to open file: errno %d\n", errno); }

#endif
    return 0;
}
