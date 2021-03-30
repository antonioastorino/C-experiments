//
//  main.c
//  errors-in-c
//
//  Created by Antonio Astorino on 27/03/2021.
//

#include "fs_utils.h"
#include "result.h"
#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // insert code here...
    Result_float d = divide(324.0, 4.3);

    FILE* file_1 = fopen("/Volumes/DataMBP/deleteme/file1.txt", "w+");
    if (file_1 == NULL) { fprintf(stderr, "Failed to open file: errno %d\n", errno); }

    Result_void folder = fs_utils_mkdir("/Volumes/DataMBP/test-folder", 0666);

    if (folder.is_err) { printf("Err: %s\n", folder.err); }

    if (d.is_err) {
        printf("Err: %s\n", d.err);
    } else {
        printf("Result: %f\n", d.ok);
    }
    return 0;
}
