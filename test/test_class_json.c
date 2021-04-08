#include "class_json.h"
#include "common.h"
#include <stdio.h>

void test_class_json()
{
    PRINT_BANNER("Test Json class")
    FILE* json_file = fopen("test/assets/test_json.json", "r");
    int c;
    size_t chars_read = 0;
    size_t size       = 4096;
    char* buf         = malloc(size);
    if (buf == NULL)
    {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    while ((c = getc(json_file)) != EOF)
    {
        if (chars_read >= size - 1)
        {
            /* time to make it bigger */
            size = (size_t)(size * 1.5);
            buf  = realloc(buf, size);
            if (buf == NULL)
            {
                fprintf(stderr, "out of memory\n");
                exit(1);
            }
        }
        buf[chars_read++] = c;
    }
    buf[chars_read++] = '\0';
    fclose(json_file);
    String* json_string_p = unwrap(String_new(buf));
    free(buf);
    buf = NULL;
    String_println(json_string_p);
    JsonObj* json_obj_p = unwrap(JsonObj_new_from_string_p(json_string_p));
    String_destroy(json_string_p); // We can delete it.
    printf("Value: %s\n", get_value_char_p(json_obj_p->root_p, "text_key"));
    printf("Value: %s\n", get_value_char_p(json_obj_p->root_p, "text_sibling"));
    // String_println(json_obj_p->tokens_string_p);
    JsonObj_destroy(json_obj_p);
}