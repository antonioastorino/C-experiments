#include "class_json.h"
#include "converter.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h> // contains free()
#include <string.h>

typedef enum
{
    NUMBER,
    STRING,
    CHILD,
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

void skip_to_next(char* location_p /* start location in the haystack */,
                  char needle /* char to search for */)
{
    while ((location_p != NULL) && *(location_p++) != needle)
    {
        printf("%c ", *location_p);
    }
    return;
}

ElementType get_value_type(char* initial_char_p)
{
    if (((initial_char_p[0] == '{') || (initial_char_p[0] == ',')) && (initial_char_p[1] == '"'))
    {
        return CHILD;
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

// bool validate_tokens_str(const char* tokens_str_p) { while () }
String* strip_whitespace(const String* json_string_p)
{
    String* ret_tokens_string_p;
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
    String* ret_string_p        = unwrap(String_new(ret_cleaned_char_p));
    return ret_string_p;
}

char* point_after(char* current_pos, const char* sequence)
{
    size_t sequence_length = strlen(sequence);
    while (&current_pos[sequence_length - 1] != NULL)
    {
        size_t i = 0;
        for (; i < sequence_length; i++)
        {
            if (current_pos[i] != sequence[i])
            {
                break;
            }
        }
        if (i == sequence_length)
        {
            return current_pos + sequence_length;
        }
        // Increment before returning
        current_pos++;
    }
    printf("Reached string end without finding %s\n", sequence);
    exit(1);
}

void print_until_quotes(const char* char_p)
{
    for (size_t i = 0; char_p[i] != '"'; i++)
    {
        printf("%c", char_p[i]);
    }
    printf("\n");
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

    return unwrap(String_new(ret_tokens_char_p));
}

Result_JsonObj_p JsonObj_new_from_string_p(const String* json_string_p)
{
    JsonObj* json_obj_p       = (JsonObj*)MALLOC(sizeof(JsonObj));
    json_obj_p->json_string_p = strip_whitespace(json_string_p);
    if ((json_obj_p->json_string_p->str[0]
         != '{') /*&& (json_obj_p->json_string_p->str[0] != '[')*/)
    {
        // TODO: Handle case in which the JSON string starts with [{ (array of objects).
        JsonObj_destroy(json_obj_p);
        // TODO: error code.
        return Err(json_obj_p, "Invalid JSON string.", -1);
    }

    char* curr_pos_p      = json_obj_p->json_string_p->str; // position analyzed (iterator)
    JsonItem* curr_item_p = NULL;
    bool in_array         = false;
    size_t array_index    = 0;

    printf("\n\nStarting\n\n");
    while ((curr_pos_p[0] != '\0') && (curr_pos_p[1] != '\0'))
    {
        if (curr_pos_p[0] == '}')
        {
            // Object ends - go to parent
            curr_item_p = curr_item_p->parent;
            curr_pos_p++;
            // Use continue to make sure the next 2 chars are checked.
            continue;
        }
        if (curr_pos_p[0] == '[')
        {
            in_array = true;
            curr_pos_p++;
            continue;
        }
        else if (curr_pos_p[0] == ']')
        {
            // Array ends - go to parent
            in_array    = false;
            array_index = 0;
            curr_item_p = curr_item_p->parent;
            curr_pos_p++;
            // Use continue to make sure the next 2 chars are checked.
            continue;
        }
        if (in_array)
        {
            // Create an item at each iteration with NULL key and incrementing index.
            JsonItem* new_item     = (JsonItem*)MALLOC(sizeof(JsonItem));
            new_item->value        = (JsonValue*)MALLOC(sizeof(JsonValue));
            new_item->next_sibling = NULL;
            if (array_index == 0)
            {
                new_item->parent                  = curr_item_p;
                curr_item_p->value->value_child_p = new_item;
                curr_item_p->value->value_type    = VALUE_ARRAY;
            }
            else if (*curr_pos_p == ',')
            {
                // It's a sibling of the current item.
                new_item->parent          = curr_item_p->parent;
                curr_item_p->next_sibling = new_item;
                curr_pos_p++;
            }
            else
            {
                JsonObj_destroy(json_obj_p);
                return Err(json_obj_p, "Invalid array", 15);
            }
            new_item->index = array_index++;
            new_item->key_p = NULL;
            curr_item_p     = new_item;
        }
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
                }
            }
            else
            {
                Result_int parsed_int = str_to_int(num_buff);
                if (!parsed_int.is_err)
                {
                    curr_item_p->value->value_type = VALUE_INT;
                    curr_item_p->value->value_int  = unwrap(parsed_int);
                }
            }
            break;
        }
        case STRING:
        {
            curr_item_p->value->value_type   = VALUE_STR;
            curr_item_p->value->value_char_p = curr_pos_p + 1; // Point after the quote
            curr_pos_p                       = terminate_str(curr_pos_p);
            // printf("Got string:\t%s\n", curr_item_p->value->value_char_p);
            // printf("Remaining string: %s\n", curr_pos_p);
            break;
        }
        case CHILD:
        {
            JsonItem* new_item     = (JsonItem*)MALLOC(sizeof(JsonItem));
            new_item->value        = (JsonValue*)MALLOC(sizeof(JsonValue));
            new_item->next_sibling = NULL;

            if (*curr_pos_p == '{')
            {
                // It's a child
                // printf("Found item\n");
                if (curr_item_p == NULL)
                {
                    // This is the root - create it instead of adding a child.
                    // printf("Creating root.\n");
                    json_obj_p->root_p = new_item;
                }
                else
                {
                    new_item->parent                  = curr_item_p;
                    curr_item_p->value->value_child_p = new_item;
                    curr_item_p->value->value_type    = VALUE_ITEM;
                }
            }
            else if (*curr_pos_p == ',')
            {
                // It's a sibling - the parent must be in common.
                // printf("Found sibling\n");
                curr_item_p->next_sibling = new_item;
                new_item->parent          = curr_item_p->parent;
            }
            // Extract the key for the new item.
            curr_item_p        = new_item;
            curr_pos_p         = curr_pos_p + 2; // That's where the key starts
            curr_item_p->key_p = curr_pos_p;
            curr_pos_p         = terminate_str(curr_pos_p); // We should be at ':' now.
            if (*curr_pos_p != ':')
            {
                printf("something bad happened\n");
                exit(4);
            }
            curr_pos_p++; // Skip the ':'.
            // printf("Got key:\t%s\n", curr_item_p->key_p);
            // printf("Remaining string: %s\n", curr_pos_p);
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
    printf("\n\ndone\n");

    return Ok(json_obj_p);
}

void JsonItem_destroy(JsonItem* json_item)
{
    if (json_item == NULL)
    {
        return;
    }
    if (json_item->value->value_type == VALUE_ITEM)
    {
        JsonItem_destroy(json_item->value->value_child_p);
    }
    if (json_item->value->value_type == VALUE_ARRAY)
    {
        JsonItem_destroy(json_item->value->value_array_p->element);
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

void get_value_char_p(const JsonItem* item, const char* key, const char** out_value)
{
    if (item == NULL)
    {
        *out_value = NULL;
        return;
    }
    if (!strcmp(item->key_p, key))
    {
        if (item->value->value_type == VALUE_STR)
        {
            *out_value = item->value->value_char_p;
            return;
        }
        else
        {
            exit(6);
        }
    }
    else
    {
        return get_value_char_p(item->next_sibling, key, out_value);
    }
};

void get_value_int(const JsonItem* item, const char* key, int* out_value)
{
    if (item == NULL)
    {
        *out_value = 0;
        return;
    }
    if (!strcmp(item->key_p, key))
    {
        if (item->value->value_type == VALUE_INT)
        {
            *out_value = item->value->value_int;
            return;
        }
        else
        {
            exit(6);
        }
    }
    else
    {
        return get_value_int(item->next_sibling, key, out_value);
    }
};
void get_value_float(const JsonItem* item, const char* key, float* out_value)
{
    if (item == NULL)
    {
        *out_value = 0.0f;
        return;
    }
    if (!strcmp(item->key_p, key))
    {
        if (item->value->value_type == VALUE_FLOAT)
        {
            *out_value = item->value->value_float;
            return;
        }
        else
        {
            exit(6);
        }
    }
    else
    {
        return get_value_float(item->next_sibling, key, out_value);
    }
};

void get_value_child_p(const JsonItem* item, const char* key, JsonItem** out_value)
{
    if (item == NULL)
    {
        *out_value = NULL;
        return;
    }
    if (!strcmp(item->key_p, key))
    {
        if (item->value->value_type == VALUE_ITEM)
        {
            *out_value = item->value->value_child_p;
            return;
        }
        else
        {
            exit(6);
        }
    }
    else
    {
        return get_value_child_p(item->next_sibling, key, out_value);
    }
};

void get_value_array_p(const JsonItem* item, const char* key, JsonArray** out_value)
{
    if (item == NULL)
    {
        *out_value = NULL;
        return;
    }
    if (!strcmp(item->key_p, key))
    {
        if (item->value->value_type == VALUE_ARRAY)
        {
            *out_value            = item->value->value_array_p;
            (*out_value)->element = item->value->value_child_p;
            return;
        }
        else
        {
            exit(7);
        }
    }
    else
    {
        return get_value_array_p(item->next_sibling, key, out_value);
    }
};

void get_array_value_char_p(const JsonArray* json_array, size_t index, const char** out_value)
{
    if (json_array == NULL)
    {
        *out_value = NULL;
        return;
    }
    JsonItem* json_item = json_array->element;
    while (true)
    {
        if (json_item->index == index)
        {
            break;
        }
        else if (json_item->next_sibling == NULL)
        {
            *out_value = NULL;
            return;
        }
        json_item = json_item->next_sibling;
    }
    if (json_item->value->value_type != VALUE_STR)
    {
        exit(35);
    }
    *out_value = json_item->value->value_char_p;
};

void get_array_value_int(const JsonArray* json_array, size_t index, int* out_value)
{
    if (json_array == NULL)
    {
        *out_value = 0;
        return;
    }
    JsonItem* json_item = json_array->element;
    while (true)
    {
        if (json_item->index == index)
        {
            break;
        }
        else if (json_item->next_sibling == NULL)
        {
            *out_value = 0;
            return;
        }
        json_item = json_item->next_sibling;
    }
    if (json_item->value->value_type != VALUE_INT)
    {
        exit(36);
    }
    *out_value = json_item->value->value_int;
};

void get_array_value_float(const JsonArray* json_array, size_t index, float* out_value)
{
    if (json_array == NULL)
    {
        *out_value = 0.0f;
        return;
    }
    JsonItem* json_item = json_array->element;
    while (true)
    {
        if (json_item->index == index)
        {
            break;
        }
        else if (json_item->next_sibling == NULL)
        {
            *out_value = 0.0f;
            return;
        }
        json_item = json_item->next_sibling;
    }
    if (json_item->value->value_type != VALUE_FLOAT)
    {
        exit(37);
    }
    *out_value = json_item->value->value_float;
};

void get_array_value_child_p(const JsonArray* json_array, size_t index, JsonItem** out_value)
{
    if (json_array == NULL)
    {
        *out_value = NULL;
        return;
    }
    JsonItem* json_item = json_array->element;
    while (true)
    {
        if (json_item->index == index)
        {
            break;
        }
        else if (json_item->next_sibling == NULL)
        {
            *out_value = NULL;
            return;
        }
        json_item = json_item->next_sibling;
    }
    if (json_item->value->value_type != VALUE_ITEM)
    {
        exit(35);
    }
    *out_value = json_item->value->value_child_p;
};