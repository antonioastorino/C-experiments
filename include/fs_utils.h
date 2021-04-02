#ifndef FS_UTILS_H
#define FS_UTILS_H

#include "result.h"
#include <sys/types.h>

// Folders only.
Result_void fs_utils_mkdir(const char*, mode_t);
Result_void fs_utils_mkdir_p(const char*, mode_t);
Result_void fs_utils_rmdir(const char*);
// Files only.
Result_void fs_utils_rm(const char*);
// Files and folders.
Result_void fs_utils_rm_r(const char*);
#endif