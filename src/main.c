//  Created by Antonio Astorino on 27/03/2021.

#include "fs_utils.h"
#include "logger.h"
#include "result.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST 1

#if TEST
#include "../test/test_class_json.c"
#include "../test/test_class_string.c"
#include "../test/test_fs_utils.c"
#include "../test/test_logger.c"
#endif

int main(int argc, char** argv)
{
    if (argc != 1)
    {
        fprintf(stderr, "Parameters not supported");
    }
    printf("Hello from `main`\n");
    printf("You are running this program from `%s`.\n", argv[0]);
    printf("Log level: %d\n", LOG_LEVEL);
#if TEST
    // test_fs_utils();
    // test_logger();
    test_class_string();
    test_class_json();
#else
    printf("Run 'bin/build-run.sh test' to run the unit tests\n");
    // insert code here...
    FILE* file_1 = fopen("/Volumes/DataMBP/delete_me/file1.txt", "w+");
    if (file_1 == NULL)
    {
        fprintf(stderr, "Failed to open file: errno %d\n", errno);
    }

#endif
    return 0;
}
