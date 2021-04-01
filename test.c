#define STRING_SANITY_BEGIN(string_obj)                                                            \
    if (string_obj == NULL) {                                                                      \
        return Err(NULL, "Unitialized String object.", -1);                                        \
    } else if (string_obj->str == NULL) {                                                          \
        return Err(NULL, "Unitialized string in String object.", -1);                              \
    } else {
#define STRING_SANITY_END                                                                          \
    return Ok(NULL);                                                                               \
    }

STRING_SANITY_BEGIN(string_obj)


STRING_SANITY_END
