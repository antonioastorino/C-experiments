//  Created by Antonio Astorino on 27/03/2021.
#include "class_json.h"
#include "class_string.h"
#include "converter.h"
#include "fs_utils.h"
#include "logger.h"
#include "result.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc != 1)
    {
        fprintf(stderr, "Parameters not supported");
    }
    printf("Hello from `main`\n");
    printf("You are running this program from `%s`.\n", argv[0]);
    printf("Log level: %d\n", LOG_LEVEL);
#if TEST == 1
    test_fs_utils();
    test_logger();
    test_class_string();
    test_converter();
    test_class_json();

#else
    String* my_string = unwrap(String_new("Hello world!"));
    String_println((const String*)my_string);
    String_destroy(my_string);

#endif
    return 0;
}
