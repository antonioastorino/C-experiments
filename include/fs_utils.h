#ifndef FS_UTILS_H
#define FS_UTILS_H

#include "result.h"
#include "class_string.h"
#include <sys/types.h>

// Folders only.
Result_void fs_utils_mkdir(String*, mode_t);
Result_void fs_utils_mkdir_p(String*, mode_t);
Result_void fs_utils_rmdir(String*);
// Files only.
Result_void fs_utils_rm(String*);
// Files and folders.
Result_void fs_utils_rm_r(String*);
#endif