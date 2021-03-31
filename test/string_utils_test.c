
#include "string_utils.h"
#define PRINT_BANNER(title)                                                                        \
    for (int i = 0; i < strlen(title); i++) { printf("="); }                                       \
    printf("\n%s\n", title);                                                                       \
    for (int i = 0; i < strlen(title); i++) { printf("="); }                                       \
    printf("\n");

PRINT_BANNER("Testing split_string");

const char path[] = "hello/world/for/you";
char* split_path  = (char*)malloc(sizeof(char) * strlen(path));
string_split(path, '/', split_path);
printf("New string length: %d\n", strlen(split_path));
printf("The returned string is %s\n", split_path);
free(split_path);
