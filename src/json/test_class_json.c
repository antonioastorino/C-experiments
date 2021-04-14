#include "class_json.h"

#if TEST == 1
#include "converter.h"
#include "mem.h"
#include <stdlib.h> // contains free()

void test_class_json()
{
    PRINT_BANNER("Test Json class")

    FILE* json_file = fopen("test/assets/test_json.json", "r");
    int c;
    size_t chars_read = 0;
    size_t size       = 4096;
    char* buf         = MALLOC(size);
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
    FREE(buf);
    buf = NULL;

    String_println(json_string_p);

    PRINT_TEST_TITLE("Root level");

    JsonObj* json_obj_p = unwrap(JsonObj_new_from_string_p(json_string_p));

    String_destroy(json_string_p); // We can delete it.
    JsonItem* json_item;           // Hold objects of type JsonItem.
    const char* value_str;         // Hold objects of type string.
    int value_int;                 // Hold objects of type int.
    float value_float;             // Hold objects of type float.
    JsonArray* json_array;         // Hold objects of type JsonArray.
    JsonItem_get(json_obj_p->root_p, "text_key", &value_str);
    ASSERT_EQ(strcmp("text_value", value_str), 0, "String value found in first item");

    JsonItem_get(json_obj_p->root_p, "text_sibling", &value_str);
    ASSERT_EQ(strcmp("sibling_value", value_str), 0, "String value found in sibling");

    PRINT_TEST_TITLE("Second level");
    JsonItem_get(json_obj_p->root_p, "nested_1", &json_item);
    ASSERT_EQ(strcmp(json_item->key_p, "object_1.1"), 0, "Found nested object key");

    JsonItem_get(json_item, "object_1.1", &value_str);
    ASSERT_EQ(strcmp(value_str, "item_1.1"), 0, "Found nested object value");
    JsonItem_get(json_item, "object_1.2", &value_str);
    ASSERT_EQ(strcmp(value_str, "item_1.2"), 0, "Found nested sibling object value");

    JsonItem_get(json_item, "object_32", &value_str);
    ASSERT_EQ(value_str, NULL, "Returned NULL for key not found");

    JsonItem_get(json_obj_p->root_p, "nested_2", &json_item);
    JsonItem_get(json_item, "object_2.1", &value_str);
    ASSERT_EQ(strcmp(value_str, "item_2.1"), 0, "Found nested object value");

    PRINT_TEST_TITLE("Third level");
    JsonItem_get(json_item, "object_2.2", &json_item);
    ASSERT_EQ(strcmp(json_item->key_p, "item_2.2"), 0, "Found nested object key");
    JsonItem_get(json_item, "item_2.2", &value_str);
    ASSERT_EQ(strcmp(value_str, "value_2.2.1"), 0, "Found nested sibling object value");

    PRINT_TEST_TITLE("Test integer");
    JsonItem_get(json_obj_p->root_p, "test_integer", &value_int);
    ASSERT_EQ(value_int, 435234, "Integer found and read correctly");

    PRINT_TEST_TITLE("Test float");
    JsonItem_get(json_obj_p->root_p, "test_float", &value_float);
    ASSERT_EQ(value_float, 435.234f, "Float found and read correctly");

    JsonItem_get(json_obj_p->root_p, "test_array", &json_array);
    JsonArray_get(json_array, 2, &value_str);
    printf("%s\n", value_str);

    JsonObj_destroy(json_obj_p);
}
#endif