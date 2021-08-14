//  Created by Antonio Astorino on 27/03/2021.
#include "class_json.h"
#include "class_string.h"
#include "converter.h"
#include "fs_utils.h"
#include "logger.h"

int main(int argc, char** argv)
{
    if (argc != 1)
    {
        fprintf(stderr, "Parameters not supported");
    }
    printf("Hello from `main-test`\n");
    printf("You are running this program from `%s`.\n", argv[0]);
    printf("Log level: %d\n", LOG_LEVEL);

    test_fs_utils();
    test_logger();
    test_class_string();
    test_converter();
    test_class_json();
    return 0;
}
