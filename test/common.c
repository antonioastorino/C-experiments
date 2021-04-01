#define PRINT_BANNER(title)                                                                        \
    for (size_t i = 0; i < strlen(title); i++) { printf("="); }                                       \
    printf("\n%s\n", title);                                                                       \
    for (size_t i = 0; i < strlen(title); i++) { printf("="); }                                       \
    printf("\n");