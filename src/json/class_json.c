#include "class_json.h"
#include "common.h"
#include "converter.h"
#include "logger.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h> // contains free()
#include <string.h>

typedef enum
{
    NUMBER,
    STRING,
    KEY,
    INVALID,
} ElementType;

bool is_token(const char c)
{
    char* token_list = "{}[],:\"";
    for (size_t i = 0; i < strlen(token_list); i++)
    {
        if (c == token_list[i])
            return true;
    }
    return false;
}

ElementType get_value_type(char* initial_char_p)
{
    if (((initial_char_p[0] == '{') || (initial_char_p[0] == ',')) && (initial_char_p[1] == '"'))
    {
        return KEY;
    }
    else if (initial_char_p[0] == '"')
    {
        return STRING;
    }
    else
    {
        return NUMBER;
    }
}

String* strip_whitespace(const String* json_string_p)
{
    // The returned string cannot be longer than the input string (plus an termination char).
    char ret_cleaned_char_p[json_string_p->length + 1];

    size_t pos_out     = 0;
    bool inside_string = false;
    for (size_t pos_in = 0; pos_in < json_string_p->length; pos_in++)
    {
        char curr_char = json_string_p->str[pos_in];
        // "Open/Close" a string.
        if (curr_char == '\"')
        { // TODO: check how it works if the value contains an escaped '\"'
            inside_string = !inside_string;
        }
        // Ignore everything outside a string that is not printable or is a space (ascii 32).
        if (inside_string || (curr_char > 32))
        {
            ret_cleaned_char_p[pos_out++] = json_string_p->str[pos_in];
        }
    }
    ret_cleaned_char_p[pos_out] = '\0';
    String* ret_string_p        = String_new(ret_cleaned_char_p);
    return ret_string_p;
}

char* terminate_str(char* char_p)
{
    while ((char_p != NULL) && (char_p + 1 != NULL))
    {
        if (*char_p == '"')
        {
            char_p++;
            if ((*char_p == ',') || (*char_p == '}') || (*char_p == ']') || (*char_p == ':'))
            {
                *(char_p - 1) = '\0';
                // return the position following the str termination
                return char_p;
            }
        }
        else
        {
            char_p++;
        }
    }
    return NULL;
}

String* generate_tokens(String* json_string_p)
{
    char ret_tokens_char_p[json_string_p->length];

    size_t pos_out     = 0;
    bool inside_string = false;
    for (size_t pos_in = 0; pos_in < json_string_p->length; pos_in++)
    {
        char curr_char = json_string_p->str[pos_in];
        // "Open/Close" a string.
        if (curr_char == '\"')
        {
            inside_string = !inside_string;
        }
        // Ignore tokens found inside a string unless it's the '\"", hence end of the string.
        if ((is_token(curr_char) && !inside_string) || curr_char == '"')
        {
            ret_tokens_char_p[pos_out++] = json_string_p->str[pos_in];
        }
    }
    ret_tokens_char_p[pos_out] = '\0';

    return String_new(ret_tokens_char_p);
}

void deserialize(JsonItem* curr_item_p, char** start_pos_p)
{
    char* curr_pos_p = *start_pos_p;
    bool parent_set  = false;
    while ((curr_pos_p[0] != '\0') && (curr_pos_p[1] != '\0'))
    {

        if ((curr_pos_p[0] == '}') || (curr_pos_p[0] == ']'))
        {
            // Use continue to make sure the next 2 chars are checked.
            curr_pos_p++;
            curr_item_p = curr_item_p->parent;
            continue;
        }

        if (curr_pos_p[0] == '[')
        {
            LOG_TRACE("Found beginning of array.");
            JsonItem* new_item                = (JsonItem*)MALLOC(sizeof(JsonItem));
            new_item->value                   = (JsonValue*)MALLOC(sizeof(JsonValue));
            new_item->index                   = 0;
            new_item->key_p                   = NULL;
            new_item->parent                  = curr_item_p;
            new_item->next_sibling            = NULL;
            curr_item_p->value->value_type    = VALUE_ARRAY;
            curr_item_p->value->value_child_p = new_item;
            curr_pos_p++;

            curr_item_p = new_item;
            continue;
        }

        if ((curr_item_p->parent->value->value_type == VALUE_ARRAY) && (*curr_pos_p == ','))
        {
            // This is a sibling of an array.
            LOG_TRACE("Found sibling in array.");
            JsonItem* new_item        = (JsonItem*)MALLOC(sizeof(JsonItem));
            new_item->value           = (JsonValue*)MALLOC(sizeof(JsonValue));
            new_item->index           = curr_item_p->index + 1;
            new_item->key_p           = NULL;
            new_item->parent          = curr_item_p->parent;
            new_item->next_sibling    = NULL;
            curr_item_p->next_sibling = new_item;
            curr_pos_p++;
            curr_item_p = new_item;
            continue;
        }

        // If we are here, it's an item. If its parent is of type VALUE_ARRAY, we need to increment
        // the index, somehow.
        switch (get_value_type(curr_pos_p))
        {
        case NUMBER:
        {
            // 23 digits should be sufficient.
            const size_t max_num_len = 23;
            char num_buff[max_num_len];
            // Try to convert into an integer or a float, depending on the presence of a dot ('.').
            bool dot_found = false;
            size_t i       = 0;
            // Create a substring containing the number.
            for (; (i < max_num_len - 1) && (*curr_pos_p != ',') && (*curr_pos_p != '}')
                   && (*curr_pos_p != ']');
                 i++)
            {
                if (*curr_pos_p == '.')
                {
                    dot_found = true;
                }
                num_buff[i] = *curr_pos_p;
                curr_pos_p++;
            }
            // Null terminate the string.
            num_buff[i] = '\0';
            if (dot_found)
            {
                Result_float parsed_float = str_to_float(num_buff);
                if (!parsed_float.is_err)
                {
                    curr_item_p->value->value_type  = VALUE_FLOAT;
                    curr_item_p->value->value_float = unwrap(parsed_float);
                    LOG_TRACE("Found value %f", curr_item_p->value->value_float);
                }
            }
            else if (num_buff[0] == '-')
            { // Convert into an integer if it is negative.
                Result_int parsed_int = str_to_int(num_buff);
                if (!parsed_int.is_err)
                {
                    curr_item_p->value->value_type = VALUE_INT;
                    curr_item_p->value->value_int  = unwrap(parsed_int);
                    LOG_TRACE("Found value %d", curr_item_p->value->value_int);
                }
            }
            else
            {
                // Convert any positive value into a size_t.
                Result_uint parsed_uint = str_to_uint(num_buff);
                if (!parsed_uint.is_err)
                {
                    curr_item_p->value->value_type = VALUE_UINT;
                    curr_item_p->value->value_uint = unwrap(parsed_uint);
                    LOG_TRACE("Found value %lu", curr_item_p->value->value_uint);
                }
            }
            break;
        }
        case STRING:
        {
            curr_item_p->value->value_type   = VALUE_STR;
            curr_item_p->value->value_char_p = curr_pos_p + 1; // Point after the quote
            curr_pos_p                       = terminate_str(curr_pos_p);
            LOG_TRACE("Found value \"%s\"", curr_item_p->value->value_char_p);
            break;
        }
        case KEY:
        {
            if (*curr_pos_p == '{')
            {
                if (parent_set)
                {
                    // It's a child
                    LOG_TRACE("Found new object");
                    JsonItem* new_item                = (JsonItem*)MALLOC(sizeof(JsonItem));
                    new_item->value                   = (JsonValue*)MALLOC(sizeof(JsonValue));
                    new_item->next_sibling            = NULL;
                    new_item->parent                  = curr_item_p;
                    curr_item_p->value->value_type    = VALUE_ITEM;
                    curr_item_p->value->value_child_p = new_item;
                    curr_item_p                       = new_item;
                }
                else
                {
                    parent_set = true;
                }
            }
            else if (*curr_pos_p == ',')
            {
                // It's a sibling - the parent must be in common.
                JsonItem* new_item        = (JsonItem*)MALLOC(sizeof(JsonItem));
                new_item->value           = (JsonValue*)MALLOC(sizeof(JsonValue));
                new_item->next_sibling    = NULL;
                curr_item_p->next_sibling = new_item;
                new_item->parent          = curr_item_p->parent;
                curr_item_p               = new_item;
            }
            // Extract the key for the new item.
            curr_pos_p         = curr_pos_p + 2; // That's where the key starts
            curr_item_p->key_p = curr_pos_p;
            curr_pos_p         = terminate_str(curr_pos_p); // We should be at ':' now.
            LOG_TRACE("Found key: \"%s\"", curr_item_p->key_p);
            if (*curr_pos_p != ':')
            {
                printf("something bad happened\n");
                exit(ERR_FATAL);
            }
            curr_pos_p++; // Skip the ':'.
            break;
        }
        case INVALID:
        {
            printf("To be handled\n");
            exit(3);
        }

        default:
            break;
        }
    }
}

// TODO: Create _Generic and unit tests.
Result_JsonObj_p JsonObj_new_from_char_p(const char* json_char_p)
{
    Result_JsonObj_p ret_result;
    String* json_string = String_new(json_char_p);
    ret_result          = JsonObj_new(json_string);
    String_destroy(json_string);
    return ret_result;
}

Result_JsonObj_p JsonObj_new_from_string_p(const String* json_string_p)
{
    JsonObj* json_obj_p = NULL;
    if (json_string_p->length == 0)
    {
        LOG_ERROR("Empty JSON string detected");
        return Err(json_obj_p, "Empty string not allowed here;", ERR_EMPTY_STRING);
    }
    String* trimmed_json_string_p = strip_whitespace(json_string_p);
    if ((trimmed_json_string_p->str[0] != '{') /*&& (json_obj_p->json_string_p->str[0] != '[')*/)
    {
        // TODO: Handle case in which the JSON string starts with [{ (array of objects).
        String_destroy(trimmed_json_string_p);
        return Err(json_obj_p, "Invalid JSON string.", ERR_JSON_INVALID);
    }
    json_obj_p                = (JsonObj*)MALLOC(sizeof(JsonObj));
    json_obj_p->json_string_p = trimmed_json_string_p;

    char* curr_pos_p          = json_obj_p->json_string_p->str; // position analyzed (iterator)
    JsonItem* curr_item_p     = (JsonItem*)MALLOC(sizeof(JsonItem));
    curr_item_p->value        = (JsonValue*)MALLOC(sizeof(JsonValue));
    curr_item_p->parent       = curr_item_p; // Set the parent to itself to recognize 'root'.
    curr_item_p->next_sibling = NULL;
    json_obj_p->root_p        = curr_item_p;

    LOG_DEBUG("JSON deserialization started.");
    deserialize(curr_item_p, &curr_pos_p);
    LOG_DEBUG("JSON deserialization ended successfully.")

    return Ok(json_obj_p);
}

void JsonItem_destroy(JsonItem* json_item)
{
    if (json_item == NULL)
    {
        return;
    }
    if (json_item->value)
    {
        if (json_item->value->value_type == VALUE_ITEM)
        {
            JsonItem_destroy(json_item->value->value_child_p);
        }
        if (json_item->value->value_type == VALUE_ARRAY)
        {
            JsonItem_destroy(json_item->value->value_child_p);
        }
    }
    if (json_item->next_sibling != NULL)
    {
        JsonItem_destroy(json_item->next_sibling);
    }
    FREE(json_item->value);
    json_item->value = NULL;
    FREE(json_item);
    json_item = NULL;
}

void JsonObj_destroy(JsonObj* json_obj_p)
{
    if (json_obj_p == NULL)
    {
        return;
    }
    if (json_obj_p->root_p != NULL)
    {
        JsonItem_destroy(json_obj_p->root_p);
    }
    String_destroy((String*)json_obj_p->json_string_p);
    FREE(json_obj_p);
    json_obj_p = NULL;
}

#define GET_VALUE_c(suffix, value_token, out_type, ACTION)                                         \
    Result_void_p get_##suffix(const JsonItem* item, const char* key, out_type out_value)          \
    {                                                                                              \
        if (item == NULL)                                                                          \
        {                                                                                          \
            *out_value = NULL;                                                                     \
            return Err(NULL, "Input item is NULL", ERR_JSON_MISSING_ENTRY);                        \
        }                                                                                          \
        if (!strcmp(item->key_p, key))                                                             \
        {                                                                                          \
            if (item->value->value_type == value_token)                                            \
            {                                                                                      \
                *out_value = item->value->suffix;                                                  \
                ACTION;                                                                            \
                return Ok(NULL);                                                                   \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                return Err(NULL, "Requested " #value_token " for a different value type.",         \
                           ERR_TYPE_MISMATCH);                                                     \
            }                                                                                      \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            return get_##suffix(item->next_sibling, key, out_value);                               \
        }                                                                                          \
    }

#define GET_NUMBER_c(suffix, value_token, out_type, ACTION)                                        \
    Result_void_p get_##suffix(const JsonItem* item, const char* key, out_type out_value)          \
    {                                                                                              \
        if (item == NULL)                                                                          \
        {                                                                                          \
            return Err(NULL, "Input item is NULL", ERR_JSON_MISSING_ENTRY);                        \
        }                                                                                          \
        if (!strcmp(item->key_p, key))                                                             \
        {                                                                                          \
            if (item->value->value_type == value_token)                                            \
            {                                                                                      \
                *out_value = item->value->suffix;                                                  \
                ACTION;                                                                            \
                return Ok(NULL);                                                                   \
            }                                                                                      \
            else if ((item->value->value_type == VALUE_INT) && (value_token == VALUE_FLOAT))       \
            {                                                                                      \
                LOG_WARN("Converting int to float");                                               \
                *out_value = (float)(1.0f * item->value->value_int);                               \
                ACTION;                                                                            \
                return Ok(NULL);                                                                   \
            }                                                                                      \
            else if ((item->value->value_type == VALUE_UINT) && (value_token == VALUE_FLOAT))      \
            {                                                                                      \
                LOG_WARN("Converting size_t to float");                                            \
                *out_value = (float)(1.0f * item->value->value_uint);                              \
                ACTION;                                                                            \
                return Ok(NULL);                                                                   \
            }                                                                                      \
            else if ((item->value->value_type == VALUE_INT) && (value_token == VALUE_UINT))        \
            {                                                                                      \
                LOG_WARN("Converting int to size_t");                                              \
                if (item->value->value_int < 0)                                                    \
                {                                                                                  \
                    LOG_ERROR("Impossible to convert negative int %d into size_t",                 \
                              item->value->value_int);                                             \
                    return Err(NULL, "Failed to convert from INT to SIZE_T", ERR_INVALID_INPUT);   \
                };                                                                                 \
                *out_value = (size_t)item->value->value_int;                                       \
                ACTION;                                                                            \
                return Ok(NULL);                                                                   \
            }                                                                                      \
            else if ((item->value->value_type == VALUE_UINT) && (value_token == VALUE_INT))        \
            {                                                                                      \
                LOG_WARN("Converting size_t to int");                                              \
                *out_value = (int)item->value->value_uint;                                         \
                /* check for overflow */                                                           \
                if ((size_t)*out_value != item->value->value_uint)                                 \
                {                                                                                  \
                    LOG_ERROR("Overflow while converting %lu into an int",                         \
                              item->value->value_uint);                                            \
                    return Err(NULL, "Failed to convert from SIZE_T to INT", ERR_INVALID_INPUT);   \
                };                                                                                 \
                ACTION;                                                                            \
                return Ok(NULL);                                                                   \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                return Err(NULL, "Requested " #value_token " for a different value type.",         \
                           ERR_TYPE_MISMATCH);                                                     \
            }                                                                                      \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            return get_##suffix(item->next_sibling, key, out_value);                               \
        }                                                                                          \
    }

#define GET_ARRAY_VALUE_c(suffix, value_token, out_type)                                           \
    Result_void_p get_array_##suffix(const JsonArray* json_array, size_t index,                    \
                                     out_type out_value)                                           \
    {                                                                                              \
        if (json_array == NULL)                                                                    \
        {                                                                                          \
            return Err(NULL, "Input item is NULL", ERR_JSON_MISSING_ENTRY);                        \
        }                                                                                          \
        JsonItem* json_item = json_array->element;                                                 \
        while (true)                                                                               \
        {                                                                                          \
            if (json_item->index == index)                                                         \
            {                                                                                      \
                break;                                                                             \
            }                                                                                      \
            else if (json_item->next_sibling == NULL)                                              \
            {                                                                                      \
                LOG_WARN("Index %lu out of boundaries.", index);                                   \
                return Err(NULL, "Input item is NULL", ERR_NULL);                                  \
            }                                                                                      \
            json_item = json_item->next_sibling;                                                   \
        }                                                                                          \
        if (json_item->value->value_type != value_token)                                           \
        {                                                                                          \
            LOG_ERROR("Incompatible data type - found %d, requested %d",                           \
                      json_item->value->value_type, value_token);                                  \
            return Err(NULL, "Data type mismatch.", ERR_TYPE_MISMATCH);                            \
        }                                                                                          \
        *out_value = json_item->value->suffix;                                                     \
        return Ok(NULL);                                                                           \
    }

GET_VALUE_c(value_char_p, VALUE_STR, const char**, );
GET_VALUE_c(value_child_p, VALUE_ITEM, JsonItem**, );
GET_VALUE_c(value_array_p, VALUE_ARRAY, JsonArray**,
            (*out_value)->element = item->value->value_child_p);

GET_NUMBER_c(value_int, VALUE_INT, int*, );
GET_NUMBER_c(value_uint, VALUE_UINT, size_t*, );
GET_NUMBER_c(value_float, VALUE_FLOAT, float*, );

GET_ARRAY_VALUE_c(value_char_p, VALUE_STR, const char**);
GET_ARRAY_VALUE_c(value_int, VALUE_INT, int*);
GET_ARRAY_VALUE_c(value_uint, VALUE_UINT, size_t*);
GET_ARRAY_VALUE_c(value_float, VALUE_FLOAT, float*);
GET_ARRAY_VALUE_c(value_child_p, VALUE_ITEM, JsonItem**);

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
    size_t value_uint;       // Hold objects of type size_t.
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
    Json_get(json_obj_p->root_p, "missing key", &value_str);
    ASSERT_EQ(value_str == NULL, true, "Returned null due to missing key.");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("Sibling key-value pair");
    json_char_p = " {\"key\": \"value string\", \"sibling\": 56}";
    json_obj_p  = unwrap(JsonObj_new(json_char_p));
    Json_get(json_obj_p->root_p, "key", &value_str);
    Json_get(json_obj_p->root_p, "sibling", &value_uint);
    ASSERT_EQ("value string", value_str, "Key for root value STRING");
    ASSERT_EQ(56, value_uint, "Key for root found with correct value INT");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("Simple array");
    json_char_p = " {\"key\": [\"array value\", 56]}";
    printf("\n%s\n", json_char_p);
    json_obj_p = unwrap(JsonObj_new(json_char_p));
    Json_get(json_obj_p->root_p, "key", &json_array);
    Json_get(json_array, 0, &value_str);
    ASSERT_EQ("array value", value_str, "Array STRING element retrieved.");
    Json_get(json_array, 1, &value_uint);
    ASSERT_EQ(56, value_uint, "Array SIZE_T element retrieved.");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("Array of objects");
    json_char_p = " {\"key\": [ {\"array key\": 56}]}";
    printf("\n%s\n", json_char_p);
    json_obj_p = unwrap(JsonObj_new(json_char_p));
    Json_get(json_obj_p->root_p, "key", &json_array);
    Json_get(json_array, 0, &json_item);
    ASSERT_EQ(json_item->key_p, "array key", "Array STRING element retrieved.");
    printf("%d\n", json_item->value->value_type);
    Json_get(json_item, "array key", &value_uint);
    ASSERT_EQ(value_uint, 56, "Value found an item that is also array element.");
    JsonObj_destroy(json_obj_p);

    PRINT_TEST_TITLE("test_json_array_1.json");
    json_string_p = load_file("test/assets/test_json_array_1.json");
    json_obj_p    = unwrap(JsonObj_new(json_string_p));
    Json_get(json_obj_p->root_p, "array_key", &json_array);
    ASSERT_EQ(json_array != NULL, true, "Array found as root element.");
    Json_get(json_array, 0, &json_item);
    ASSERT_EQ(json_item != NULL, true, "First array element is an item.");
    Json_get(json_item, "object 1", &value_uint);
    ASSERT_EQ(value_uint, 56, "Value SIZE_T found");
    Json_get(json_array, 1, &json_item);
    ASSERT_EQ(json_item != NULL, true, "Second array element is an item.");
    Json_get(json_item, "object 2", &value_float);
    ASSERT_EQ(value_float, 404.5f, "Value FLOAT found");
    Json_get(json_array, 2, &json_item);
    Json_get(json_item, "object 3", &value_str);
    ASSERT_EQ(value_str, "SOME STRING", "Array element STRING found.");
    Json_get(json_array, 3, &value_uint);
    ASSERT_EQ(value_uint, 32, "Array element INT found.");
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
    Json_get(json_array_2, 0, &value_uint);
    ASSERT_EQ(value_uint, 12314, "Value SIZE_T found");
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
    Json_get(json_obj_p->root_p, "test_integer", &value_uint);
    ASSERT_EQ(value_uint, 435234, "Integer found and read correctly");

    PRINT_TEST_TITLE("Test float");
    Json_get(json_obj_p->root_p, "test_float", &value_float);
    ASSERT_EQ(value_float, 435.234f, "Float found and read correctly");

    Json_get(json_obj_p->root_p, "test_array", &json_array);
    Json_get(json_array, 0, &value_uint);
    ASSERT_EQ(value_uint, 14352, "Array element of type INT read correctly");
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
    Json_get(json_obj_p->root_p, "value_int", &value_uint);
    ASSERT_EQ((size_t)23, value_uint, "Conversion from INT to SIZE_T successfull");
    Json_get(json_obj_p->root_p, "value_small_uint", &value_int);
    ASSERT_EQ((int)43, value_int, "Conversion from SIZE_T to INT successfull");
    res_void_p = Json_get(json_obj_p->root_p, "value_negative_int", &value_uint);
    ASSERT_EQ(res_void_p.is_err, true, "Conversion from negative INT to SIZE_T failed");
    res_void_p = Json_get(json_obj_p->root_p, "value_uint", &value_int);
    ASSERT_EQ(res_void_p.is_err, true, "Conversion from large SIZE_T to INT failed");
    JsonObj_destroy(json_obj_p);
    String_destroy(json_string_p);
    /**/
}
#endif
