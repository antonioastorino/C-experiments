#include "class_json.h"
#include "class_string.h"
#include "common.h"
#include "logger.h"
#include "mem.h"
#include <stdlib.h>
#if TEST == 1

String* load_file(char* filename)
{
    FILE* json_file = fopen(filename, "r");
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
    String* json_string_p = String_new(buf);
    FREE(buf);
    buf = NULL;
    return json_string_p;
}

void test_class_json()
{
    PRINT_BANNER();

    String* json_string_p;
    JsonObj* json_obj_p;
    JsonItem* json_item;     // Hold objects of type JsonItem.
    const char* value_str;   // Hold objects of type string.
    int value_int;           // Hold objects of type int.
    size_t value_size_t;     // Hold objects of type size_t.
    float value_float;       // Hold objects of type float.
    JsonArray* json_array;   // Hold objects of type JsonArray.
    JsonArray* json_array_2; // Hold objects of type JsonArray.
    const char* json_char_p;
    bool missing_entry = false;
    Result_void_p res_void_p;

    PRINT_TEST_TITLE("Key-value pair");
    json_char_p = " {\"key\": \"value string\"}";
    json_obj_p  = unwrap(JsonObj_new(json_char_p));
    Json_get(json_obj_p->root_p, "key", &value_str);
    ASSERT_EQ("value string", value_str, "Key for root found with correct value STRING");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("Sibling key-value pair");
    json_char_p = " {\"key\": \"value string\", \"sibling\": 56}";
    json_obj_p  = unwrap(JsonObj_new(json_char_p));
    Json_get(json_obj_p->root_p, "key", &value_str);
    Json_get(json_obj_p->root_p, "sibling", &value_size_t);
    ASSERT_EQ("value string", value_str, "Key for root value STRING");
    ASSERT_EQ(56, value_size_t, "Key for root found with correct value INT");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("Simple array");
    json_char_p = " {\"key\": [\"array value\", 56]}";
    printf("\n%s\n", json_char_p);
    json_obj_p = unwrap(JsonObj_new(json_char_p));
    Json_get(json_obj_p->root_p, "key", &json_array);
    Json_get(json_array, 0, &value_str);
    ASSERT_EQ("array value", value_str, "Array STRING element retrieved.");
    Json_get(json_array, 1, &value_size_t);
    ASSERT_EQ(56, value_size_t, "Array SIZE_T element retrieved.");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("Array of objects");
    json_char_p = " {\"key\": [ {\"array key\": 56}]}";
    printf("\n%s\n", json_char_p);
    json_obj_p = unwrap(JsonObj_new(json_char_p));
    Json_get(json_obj_p->root_p, "key", &json_array);
    Json_get(json_array, 0, &json_item);
    ASSERT_EQ(json_item->key_p, "array key", "Array STRING element retrieved.");
    printf("%d\n", json_item->value->value_type);
    Json_get(json_item, "array key", &value_size_t);
    ASSERT_EQ(value_size_t, 56, "Value found an item that is also array element.");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("test_json_array_1.json");
    json_string_p = load_file("test/assets/test_json_array_1.json");
    json_obj_p    = unwrap(JsonObj_new(json_string_p));
    Json_get(json_obj_p->root_p, "array_key", &json_array);
    ASSERT_EQ(json_array != NULL, true, "Array found as root element.");
    Json_get(json_array, 0, &json_item);
    ASSERT_EQ(json_item != NULL, true, "First array element is an item.");
    Json_get(json_item, "object 1", &value_size_t);
    ASSERT_EQ(value_size_t, 56, "Value SIZE_T found");
    Json_get(json_array, 1, &json_item);
    ASSERT_EQ(json_item != NULL, true, "Second array element is an item.");
    Json_get(json_item, "object 2", &value_float);
    ASSERT_EQ(value_float, 404.5f, "Value FLOAT found");
    Json_get(json_array, 2, &json_item);
    Json_get(json_item, "object 3", &value_str);
    ASSERT_EQ(value_str, "SOME STRING", "Array element STRING found.");
    Json_get(json_array, 3, &value_size_t);
    ASSERT_EQ(value_size_t, 32, "Array element INT found.");
    JsonObj_destroy(json_obj_p);
    String_destroy(json_string_p);

    PRINT_TEST_TITLE("test_json_array_2.json");
    json_string_p = load_file("test/assets/test_json_array_2.json");
    json_obj_p    = unwrap(JsonObj_new(json_string_p));
    Json_get(json_obj_p->root_p, "array_key", &json_array);
    ASSERT_EQ(json_array != NULL, true, "Array found as root element.");
    Json_get(json_array, 0, &json_item);
    ASSERT_EQ(json_item != NULL, true, "First array element is an item.");
    Json_get(json_item, "inner array 1", &json_array_2);
    Json_get(json_array_2, 0, &value_size_t);
    ASSERT_EQ(value_size_t, 12314, "Value SIZE_T found");
    Json_get(json_array_2, 1, &value_float);
    ASSERT_EQ(value_float, -32.4f, "Value FLOAT found");
    Json_get(json_array, 1, &json_item);
    ASSERT_EQ(json_item != NULL, true, "Second array element is an item.");
    Json_get(json_item, "inner array 2", &json_array_2);
    Json_get(json_array_2, 0, &value_float);
    ASSERT_EQ(value_float, 1.4f, "Value FLOAT found");
    Json_get(json_array_2, 1, &value_str);
    ASSERT_EQ(value_str, "hello", "Value STRING found");
    JsonObj_destroy(json_obj_p);
    String_destroy(json_string_p);

    PRINT_TEST_TITLE("Testing test/assets/test_json.json");
    json_string_p = load_file("test/assets/test_json.json");
    json_obj_p    = unwrap(JsonObj_new_from_string_p(json_string_p));
    String_destroy(json_string_p); // We can delete it.
    Json_get(json_obj_p->root_p, "text_key", &value_str);
    ASSERT_EQ("text_value", value_str, "String*value found in first item");

    Json_get(json_obj_p->root_p, "text_sibling", &value_str);
    ASSERT_EQ("sibling_value", value_str, "String*value found in sibling");

    PRINT_TEST_TITLE("Second level");
    Json_get(json_obj_p->root_p, "nested_1", &json_item);
    ASSERT_EQ(json_item->key_p, "object_1.1", "Found nested object key");

    Json_get(json_item, "object_1.1", &value_str);
    ASSERT_EQ(value_str, "item_1.1", "Found nested object value");
    Json_get(json_item, "object_1.2", &value_str);
    ASSERT_EQ(value_str, "item_1.2", "Found nested sibling object value");

    Result_void_p res_entry_found = Json_get(json_item, "object_32", &value_str);
    ASSERT_EQ((int)res_entry_found.err.code, ERR_JSON_MISSING_ENTRY,
              "Returned NULL for key not found");

    Json_get(json_obj_p->root_p, "nested_2", &json_item);
    Json_get(json_item, "object_2.1", &value_str);
    ASSERT_EQ(value_str, "item_2.1", "Found nested object value");

    PRINT_TEST_TITLE("Third level");
    Json_get(json_item, "object_2.2", &json_item);
    ASSERT_EQ(json_item->key_p, "item_2.2", "Found nested object key");
    Json_get(json_item, "item_2.2", &value_str);
    ASSERT_EQ(value_str, "value_2.2.1", "Found nested sibling object value");

    PRINT_TEST_TITLE("Test integer");
    Json_get(json_obj_p->root_p, "test_integer", &value_size_t);
    ASSERT_EQ(value_size_t, 435234, "Integer found and read correctly");

    PRINT_TEST_TITLE("Test float");
    Json_get(json_obj_p->root_p, "test_float", &value_float);
    ASSERT_EQ(value_float, 435.234f, "Float found and read correctly");

    Json_get(json_obj_p->root_p, "test_array", &json_array);
    Json_get(json_array, 0, &value_size_t);
    ASSERT_EQ(value_size_t, 14352, "Array element of type INT read correctly");
    Json_get(json_array, 1, &value_float);
    ASSERT_EQ(value_float, 2.15f, "Array element of type FLOAT read correctly");
    Json_get(json_array, 2, &value_str);
    ASSERT_EQ(value_str, "string_element", "Array element of type C-string read correctly");

    JsonObj_destroy(json_obj_p);

    Result_JsonObj_p res_json_obj_p;
    PRINT_TEST_TITLE("Invalid JSON string - empty")
    json_string_p  = String_new("");
    res_json_obj_p = JsonObj_new_from_string_p(json_string_p);
    ASSERT_EQ((int)res_json_obj_p.err.code, ERR_EMPTY_STRING, "Empty JSON fails to initialize.");
    String_destroy(json_string_p);

    PRINT_TEST_TITLE("Invalid JSON string - string not starting with '{'")
    // TODO: crate token analyzer and add TC's.
    json_string_p  = String_new("This is not a JSON file");
    res_json_obj_p = JsonObj_new_from_string_p(json_string_p);
    ASSERT_EQ((int)res_json_obj_p.err.code, ERR_JSON_INVALID, "Invalid JSON fails to initialize.");
    String_destroy(json_string_p);

    PRINT_TEST_TITLE("Fixed memory leak");
    json_char_p                     = "{\"request\":[\"Required parameter is missing\"]}";
    JsonObj* json_with_vector_obj_p = unwrap(JsonObj_new(json_char_p));
    Json_get(json_with_vector_obj_p->root_p, "request", &json_array);
    Json_get(json_array, 0, &value_str);
    ASSERT_EQ(value_str, "Required parameter is missing", "");
    JsonObj_destroy(json_with_vector_obj_p);

    PRINT_TEST_TITLE("Data conversion");
    json_string_p = load_file("test/assets/test_json_numbers.json");
    json_obj_p    = unwrap(JsonObj_new(json_string_p));
    Json_get(json_obj_p->root_p, "value_int", &value_size_t);
    ASSERT_EQ((size_t)23, value_size_t, "Conversion from INT to SIZE_T successfull");
    Json_get(json_obj_p->root_p, "value_small_size_t", &value_int);
    ASSERT_EQ((int)43, value_int, "Conversion from SIZE_T to INT successfull");
    res_void_p = Json_get(json_obj_p->root_p, "value_negative_int", &value_size_t);
    ASSERT_EQ(res_void_p.is_err, true, "Conversion from negative INT to SIZE_T failed");
    res_void_p = Json_get(json_obj_p->root_p, "value_size_t", &value_int);
    ASSERT_EQ(res_void_p.is_err, true, "Conversion from large SIZE_T to INT failed");
    JsonObj_destroy(json_obj_p);
    String_destroy(json_string_p);
    /**/
}
#endif
