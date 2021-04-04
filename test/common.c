#define ASSERT_EQ(value_1, value_2, message)                                                       \
    if (value_1 == value_2)                                                                        \
    {                                                                                              \
        printf("\n> \e[32mOk\e[0m\t %s\n", message);                                               \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        printf("\n> \e[31mErr - Test failed.\e[0m\n%s:%d\n", __FILE__, __LINE__);                  \
        return;                                                                                    \
    }

#define PRINT_BANNER(title)                                                                        \
    printf("\n");                                                                                  \
    for (size_t i = 0; i < strlen(title) + 12; i++)                                                \
    {                                                                                              \
        printf("=");                                                                               \
    }                                                                                              \
    printf("\n-- TEST: %s --\n", title);                                                           \
    for (size_t i = 0; i < strlen(title) + 12; i++)                                                \
    {                                                                                              \
        printf("=");                                                                               \
    }                                                                                              \
    printf("\n");

#define PRINT_TEST_TITLE(title) printf("\n------- T:%lu - %s -------\n", ++counter, title);