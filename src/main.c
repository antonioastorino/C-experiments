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
    printf("You are running `%s`.\n", argv[0]);
    printf("Log level: %d\n", LOG_LEVEL);

    return 0;
}
