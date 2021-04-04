#ifndef FS_UTILS_H
#define FS_UTILS_H

#include "class_string.h"
#include "result.h"
#include <sys/types.h>

// Folders only.
Result fs_utils_mkdir(String*, mode_t);
Result fs_utils_mkdir_p(String*, mode_t);
Result fs_utils_rmdir(String*);
// Files only.
Result fs_utils_rm(String*);
// Files and folders.
Result fs_utils_rm_r(String*);
#endif