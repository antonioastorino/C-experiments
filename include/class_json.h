#ifndef CLASS_JSON_H
#define CLASS_JSON_H
#include "class_string.h"
#include "result.h"

// A air `key` `value`, plus a `parent` to make a double-linked list, and a `sibling`.
typedef struct json_item JsonItem;
// Any possible value, including another `JsonItem`.
typedef struct json_value JsonValue;

typedef enum
{
    UNDEFINED,
    value_int,
    value_float,
    value_char_p,
    value_child_p,
    INVALID,
} ValueType;
/**
 *
 * JSON operators (if found outside a string):
 *     {"  => create an item and its key is starting
 *             :[  => create value
 *                  ,   => creates a sibling value (in array) of the same type the previous item
 *         ":  => create a single value
 *         ,"  => create a sibling key
 *     ]," => create sibling key
 *     },"  => create sibling item
 *     }   => go to the parent
 * Invalid sequences (outside strings):
 *     { not followed by "
 *     [[
 *     ,,
 *     ::
 *     :,
 *     ] not followed by , or }
 */

typedef struct json_value
{
    ValueType value_type;
    union
    {
        int value_int;                   // leaf int
        float value_float;               // leaf float
        const char* value_char_p;        // leaf c-string
        struct json_item* value_child_p; // another item
                                         // TODO: add array
    };
} JsonValue;

typedef struct json_item
{
    const char* key_p;
    JsonValue* value;
    struct json_item* parent;
    struct json_item* next_sibling;
} JsonItem;

typedef struct class_json_obj
{
    const String* json_string_p;
    JsonItem* root_p;
} JsonObj;

Result_JsonObj_p JsonObj_new_from_string_p(const String*);
void JsonObj_destroy(JsonObj*);

void JsonObj_get_tokens(String*);

#define GET_VALUE_h(suffix, out_type) void get_##suffix(const JsonItem*, const char*, out_type);

GET_VALUE_h(value_char_p, const char**);
GET_VALUE_h(value_int, int*);
GET_VALUE_h(value_child_p, JsonItem**);

// clang-format off
#define JsonItem_get(json_item_p, key, out_p)                    \
    _Generic(out_p,                                              \
        const char** : get_value_char_p,                         \
        int* : get_value_int,                                    \
        JsonItem** : get_value_child_p                           \
        )(json_item_p, key, out_p)
// clang-format on
#endif
