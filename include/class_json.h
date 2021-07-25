#ifndef CLASS_JSON_H
#define CLASS_JSON_H
#include "class_string.h"
#include "common.h"
#include "result.h"
// A air `key` `value`, plus a `parent` to make a double-linked list, and a `sibling`.
#define SET_MISSING_ENTRY(result, bool_value, success_string)                                      \
    if (result.err.code)                                                                           \
    {                                                                                              \
        LOG_ON_ERR(result, success_string);                                                        \
        bool_value = true;                                                                         \
    }

// A air `key` `value`, plus a `parent` to make a double-linked list, and a `sibling`.
typedef struct json_item JsonItem;
// Any possible value, including another `JsonItem`.
typedef struct json_value JsonValue;

typedef enum
{
    VALUE_UNDEFINED,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STR,
    VALUE_ARRAY,
    VALUE_ITEM,
    VALUE_INVALID,
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

// Used only for returning data in a convenient way. Not used for storage.
typedef struct json_array
{
    struct json_item* element;
} JsonArray;

typedef struct json_value
{
    ValueType value_type;
    union
    {
        int value_int;                    // leaf int
        float value_float;                // leaf float
        const char* value_char_p;         // leaf c-string
        struct json_item* value_child_p;  // another item
        struct json_array* value_array_p; // the first item of an array
    };
} JsonValue;

typedef struct json_item
{
    const char* key_p;
    size_t index; // For arrays only
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
Result_JsonObj_p JsonObj_new_from_char_p(const char*);
void JsonObj_destroy(JsonObj*);

void JsonObj_get_tokens(String*);

#define GET_VALUE_h(suffix, out_type)                                                              \
    Result_void_p get_##suffix(const JsonItem*, const char*, out_type);
GET_VALUE_h(value_char_p, const char**);
GET_VALUE_h(value_int, int*);
GET_VALUE_h(value_float, float*);
GET_VALUE_h(value_child_p, JsonItem**);
GET_VALUE_h(value_array_p, JsonArray**);

#define GET_ARRAY_VALUE_h(suffix, out_type)                                                        \
    Result_void_p get_array_##suffix(const JsonArray*, size_t, out_type);
GET_ARRAY_VALUE_h(value_char_p, const char**);
GET_ARRAY_VALUE_h(value_int, int*);
GET_ARRAY_VALUE_h(value_float, float*);
GET_ARRAY_VALUE_h(value_child_p, JsonItem**);

// clang-format off
#define JsonObj_new(in_json)                                     \
    _Generic(in_json,                                            \
        const char*  : JsonObj_new_from_char_p,                  \
        String*      : JsonObj_new_from_string_p                 \
        )(in_json)

#define JsonItem_get(json_item_p, key, out_p)                    \
    _Generic(out_p,                                              \
        const char** : get_value_char_p,                         \
        int* : get_value_int,                                    \
        float* : get_value_float,                                \
        JsonItem** : get_value_child_p,                          \
        JsonArray** : get_value_array_p                          \
        )(json_item_p, key, out_p)

#define JsonArray_get(json_array_p, index, out_p)                \
    _Generic(out_p,                                              \
        const char** : get_array_value_char_p,                   \
        int*         : get_array_value_int,                      \
        float*       : get_array_value_float,                    \
        JsonItem**   : get_array_value_child_p                   \
        )(json_array_p, index, out_p)
// clang-format on
#endif
#if TEST == 1
void test_class_json(void);
#endif
