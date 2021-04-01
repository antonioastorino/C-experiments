#define ASSERT_EQ(value_1, value_2, message)                                                       \
    if (value_1 == value_2) {                                                                      \
        printf("> Ok\t %s\n", message);                                                              \
    } else {                                                                                       \
        printf("> Err\t %s\n", message);                                                             \
    }

#define PRINT_BANNER(title)                                                                        \
    for (size_t i = 0; i < strlen(title); i++) { printf("="); }                                    \
    printf("\n%s\n", title);                                                                       \
    for (size_t i = 0; i < strlen(title); i++) { printf("="); }                                    \
    printf("\n");