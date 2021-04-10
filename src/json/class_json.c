#include "class_json.h"
#include <stdio.h>
#include <stdlib.h> // contains free()
#include <string.h>

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

ValueType get_value_type(char* initial_char_p)
{
    if (((initial_char_p[0] == '{') || (initial_char_p[0] == ',')) && (initial_char_p[1] == '"'))
        return JSON_ITEM;
    else if (initial_char_p[0] == '"')
        return C_STR;
    else
        printf("GOT: '%c%c'", initial_char_p[0], initial_char_p[1]);
    return INVALID;
}

// bool validate_tokens_str(const char* tokens_str_p) { while () }
String* strip_whitespace(String* json_string_p)
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

    return unwrap(String_new(ret_cleaned_char_p));
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
    JsonObj* json_obj_p       = (JsonObj*)malloc(sizeof(JsonObj));
    json_obj_p->json_string_p = strip_whitespace(unwrap(String_clone(json_string_p)));
    printf("%s\n", json_obj_p->json_string_p->str);
    if ((json_obj_p->json_string_p->str[0]
         != '{') /*&& (json_obj_p->json_string_p->str[0] != '[')*/)
    {
        // TODO: Handle case in which the JSON string starts with [{ (array of objects).
        JsonObj_destroy(json_obj_p);
        // TODO: error code.
        return Err(json_obj_p, "Invalid JSON string.", -1);
    }

    char* curr_pos      = json_obj_p->json_string_p->str; // position analyzed (iterator)
    JsonItem* curr_item = NULL;

    printf("\n\nStarting\n\n");
    while ((curr_pos[0] != '\0') && (curr_pos[1] != '\0'))
    {
        if (curr_pos[0] == '}')
        {
            // Object ends
            curr_pos++;
            // Use continue to make sure the next 2 chars are checked.
            continue;
        }
        switch (get_value_type(curr_pos))
        {
        case INT:
            curr_pos++;
            printf("got int ---------------\n");
            /* code */
            break;
        case FLOAT:
            curr_pos++;
            /* code */
            break;
        case C_STR:
            curr_item->value->value_type   = C_STR;
            curr_item->value->value_char_p = curr_pos + 1; // Point after the quote
            curr_pos                       = terminate_str(curr_pos);
            printf("Got string:\t%s\n", curr_item->value->value_char_p);
            printf("Remaining string: %s\n", curr_pos);
            break;
        case JSON_ITEM:
            printf("Got item \n");
            // Create a child
            JsonItem* new_item     = (JsonItem*)malloc(sizeof(JsonItem));
            new_item->value        = (JsonValue*)malloc(sizeof(JsonValue));
            new_item->next_sibling = NULL;

            if (*curr_pos == '{')
            {
                // It's a child
                printf("Found item\n");
                if (curr_item == NULL)
                {
                    // This is the root - create it instead of adding a child.
                    printf("Creating root.\n");
                    json_obj_p->root_p = new_item;
                }
                else
                {
                    new_item->parent             = curr_item;
                    curr_item->value->child_p    = new_item;
                    curr_item->value->value_type = JSON_ITEM;
                }
            }
            else if (*curr_pos == ',')
            {
                // It's a sibling - the parent must be in common.
                printf("Found sibling\n");
                curr_item->next_sibling = new_item;
                new_item->parent        = curr_item->parent;
            }
            // Extract the key for the new item.
            curr_item        = new_item;
            curr_pos         = curr_pos + 2; // That's where the key starts
            curr_item->key_p = curr_pos;
            curr_pos         = terminate_str(curr_pos); // We should be at ':' now.
            if (*curr_pos != ':')
            {
                printf("something bad happened\n");
                exit(4);
            }
            curr_pos++; // Skip the ':'.
            printf("Got key:\t%s\n", curr_item->key_p);
            printf("Remaining string: %s\n", curr_pos);
            break;
        case INVALID:
            printf("To be handled\n");
            exit(3);

        default:
            break;
        }
    }
    printf("\n\ndone\n");
    // json_obj_p->tokens_string_p = generate_tokens(json_obj_p->json_string_p);
    // if (!validate_json_tokens_string(tokens_string_p))
    // {
    //     // TODO: create error codes.
    //     return Err(JsonOjb*, "Invalid JSON", -1);
    // }
    return Ok(json_obj_p);
}

void JsonObj_destroy(JsonObj* json_obj_p)
{
    free(json_obj_p->json_string_p);
    json_obj_p->json_string_p = NULL;
    //	TODO: Free all children and siblings.
    free(json_obj_p);
    json_obj_p = NULL;
}

const char* get_value_char_p(const JsonItem* item, const char* key)
{
    if (item == NULL)
        return NULL;
    if (!strcmp(item->key_p, key))
    {
        // The key matches
        if (item->value->value_type == C_STR)
        {

            return item->value->value_char_p;
        }
        else
        {
            exit(6);
        }
    }
    else
    {
        // Try another key
        return get_value_char_p(item->next_sibling, key);
    }
}

JsonItem* get_value_item_p(const JsonItem* item, const char* key)
{
    if (item == NULL)
        return NULL;
    if (!strcmp(item->key_p, key))
    {
        // The key matches
        if (item->value->value_type == JSON_ITEM)
        {

            return item->value->child_p;
        }
        else
        {
            exit(6);
        }
    }
    else
    {
        // Try another key
        return get_value_item_p(item->next_sibling, key);
    }
}