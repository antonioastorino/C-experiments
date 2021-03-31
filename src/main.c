//  Created by Antonio Astorino on 27/03/2021.

#include "fs_utils.h"
#include "result.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST 1

int main() {
#if TEST
#include "../test/string_utils_test.c"
#else

#endif
    // insert code here...
    FILE* file_1 = fopen("/Volumes/DataMBP/deleteme/file1.txt", "w+");
    if (file_1 == NULL) { fprintf(stderr, "Failed to open file: errno %d\n", errno); }

    Result_void folder = fs_utils_mkdir("/Volumes/DataMBP/tt/test-folder", 0666);

    if (folder.is_err) { printf("Err: %s\n", folder.err); }

    return 0;
}
