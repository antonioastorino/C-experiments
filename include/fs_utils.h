#ifndef FS_UTILS_H
#define FS_UTILS_H

#include "class_string.h"
#include "common.h"
#include "result.h"
#include <sys/types.h>

// Folders only.
Result_void_p fs_utils_mkdir(const char*, mode_t);
Result_void_p fs_utils_mkdir_p(const char*, mode_t);
Result_void_p fs_utils_rmdir(const char*);
// Files only.
Result_void_p fs_utils_rm_from_path_as_char_p(const char*);
Result_String_p fs_utils_read_to_string_from_path_as_char_p(const char*);
Result_void_p fs_utils_append(const char*, const char*);
// Files and folders.
Result_void_p fs_utils_rm_r(const char*);

// clang-format off
/*
Overload the following function to allow the use of `const char *` or `String**` as input file path
*/
#define fs_utils_rm(file_path_p)                                        \
    _Generic((file_path_p),                                             \
        const char*   : fs_utils_rm_from_path_as_char_p,                \
        char*         : fs_utils_rm_from_path_as_char_p                 \
    )(file_path_p)

#define fs_utils_read_to_string(file_path_p)                            \
    _Generic((file_path_p),                                             \
        const char* : fs_utils_read_to_string_from_path_as_char_p,      \
        char* : fs_utils_read_to_string_from_path_as_char_p            \
    )(file_path_p)

#if TEST == 1
    void test_fs_utils(void);
#endif
#endif