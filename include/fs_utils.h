#ifndef FS_UTILS_H
#define FS_UTILS_H

#include "class_string.h"
#include "common.h"
#include "result.h"
#include <sys/types.h>

// Folders only.
Result_void_p fs_utils_mkdir(String*, mode_t);
Result_void_p fs_utils_mkdir_p(String*, mode_t);
Result_void_p fs_utils_rmdir(String*);
// Files only.
Result_void_p fs_utils_rm(String*);
// Files and folders.
Result_void_p fs_utils_rm_r(String*);

#if TEST == 1
void test_fs_utils(void);
#endif

#endif