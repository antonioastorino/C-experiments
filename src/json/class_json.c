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
    String* ret_tokens_string_p;
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
            else
            {
                Result_int parsed_int = str_to_int(num_buff);
                if (!parsed_int.is_err)
                {
                    curr_item_p->value->value_type = VALUE_INT;
                    curr_item_p->value->value_int  = unwrap(parsed_int);
                    LOG_TRACE("Found value %d", curr_item_p->value->value_int);
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
                if (curr_item_p->parent != curr_item_p)
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
    bool in_array             = false;
    size_t array_index        = 0;

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
                LOG_ERROR("Requested %s for a different value type.", #value_token);               \
                exit(ERR_TYPE_MISMATCH);                                                           \
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
                LOG_ERROR("Index out of boundaries.");                                             \
                return Err(NULL, "Input item is NULL", ERR_NULL);                                  \
            }                                                                                      \
            json_item = json_item->next_sibling;                                                   \
        }                                                                                          \
        if (json_item->value->value_type != value_token)                                           \
        {                                                                                          \
            LOG_ERROR("Incompatible data type: %d != %d", json_item->value->value_type,            \
                      value_token);                                                                \
            exit(ERR_TYPE_MISMATCH);                                                               \
        }                                                                                          \
        *out_value = json_item->value->suffix;                                                     \
        return Ok(NULL);                                                                           \
    }

GET_VALUE_c(value_char_p, VALUE_STR, const char**, );
GET_VALUE_c(value_int, VALUE_INT, int*, );
GET_VALUE_c(value_float, VALUE_FLOAT, float*, );
GET_VALUE_c(value_child_p, VALUE_ITEM, JsonItem**, );
GET_VALUE_c(value_array_p, VALUE_ARRAY, JsonArray**,
            (*out_value)->element = item->value->value_child_p);

GET_ARRAY_VALUE_c(value_char_p, VALUE_STR, const char**);
GET_ARRAY_VALUE_c(value_int, VALUE_INT, int*);
GET_ARRAY_VALUE_c(value_float, VALUE_FLOAT, float*);
GET_ARRAY_VALUE_c(value_child_p, VALUE_ITEM, JsonItem**);