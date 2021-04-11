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

int get_value_type(char* initial_char_p)
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
        switch (get_value_type(curr_pos_p))
        {
        case NUMBER:
        {
            char int_buff[23];
            // Try to convert into an integer
            for (size_t i = 0;
                 (i < 22) && (*curr_pos_p != ',') && (*curr_pos_p != '}') && (*curr_pos_p != ']');
                 i++)
            {
                int_buff[i] = *curr_pos_p;
                curr_pos_p++;
                int_buff[i + 1]       = '\0';
                Result_int parsed_int = str_to_int(int_buff);
                if (!parsed_int.is_err)
                {
                    curr_item_p->value->value_type = value_int;
                    curr_item_p->value->value_int  = unwrap(parsed_int);
                }
            }
            break;
        }
        case STRING:
        {
            curr_item_p->value->value_type   = value_char_p;
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
                    curr_item_p->value->value_type    = value_child_p;
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
    if (json_item->value->value_type == value_child_p)
    {
        JsonItem_destroy(json_item->value->value_child_p);
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

#define GET_VALUE_c(suffix, out_type)                                                              \
    void get_##suffix(const JsonItem* item, const char* key, out_type out_value)                   \
    {                                                                                              \
        if (item == NULL)                                                                          \
        { /* TODO: when the out_type is pointer, we cannot assign its pointee to NULL */           \
            *out_value = NULL;                                                                     \
            return;                                                                                \
        }                                                                                          \
        if (!strcmp(item->key_p, key))                                                             \
        {                                                                                          \
            /* The key matches */                                                                  \
            if (item->value->value_type == suffix)                                                 \
            {                                                                                      \
                *out_value = item->value->suffix;                                                  \
                return;                                                                            \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                exit(6);                                                                           \
            }                                                                                      \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            /* Try another sibling */                                                              \
            return get_##suffix(item->next_sibling, key, out_value);                               \
        }                                                                                          \
    }

GET_VALUE_c(value_char_p, const char**);
GET_VALUE_c(value_int, int*);
GET_VALUE_c(value_child_p, JsonItem**);