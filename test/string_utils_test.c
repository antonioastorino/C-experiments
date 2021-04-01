
#include "common.c"
#include "string_utils.h"

PRINT_BANNER("Testing split_string");

const char path[] = "hello/world/for/you";
char* split_path  = (char*)malloc(sizeof(char) * strlen(path));
string_split(path, '/', split_path);
printf("New string length: %lu\n", strlen(split_path));
printf("The returned string is %s\n", split_path);
free(split_path);
