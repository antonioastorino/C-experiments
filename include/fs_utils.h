#ifndef FS_UTILS_H
#define FS_UTILS_H

#include "result.h"
#include <sys/types.h>

Result_void fs_utils_mkdir(const char* dir_path, mode_t permission);
Result_void fs_utils_mkdir_p(const char* dir_path, mode_t permission);

#endif
