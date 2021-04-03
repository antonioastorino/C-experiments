#define ASSERT_EQ(value_1, value_2, message)                                                       \
    if (value_1 == value_2) {                                                                      \
        printf("\n> Ok\t %s\n", message);                                                            \
    } else {                                                                                       \
        printf("\n> Err\t Test failed.\n%s:%d\n", __FILE__, __LINE__);                               \
    }

#define PRINT_BANNER(title)                                                                        \
    printf("\n");                                                                                  \
    for (size_t i = 0; i < strlen(title) + 6; i++) { printf("="); }                                \
    printf("\n-- %s --\n", title);                                                                 \
    for (size_t i = 0; i < strlen(title) + 6; i++) { printf("="); }                                \
    printf("\n");
