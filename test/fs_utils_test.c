#include "common.c"
#include "fs_utils.h"

PRINT_BANNER("Testing fs_utils");
Result_void res_void;

res_void = fs_utils_mkdir_p("/Volumes/DataMBP/new_folder/new_inner_folder/", 0777);
if (res_void.error_code) { printf("This is not ok\n"); }

res_void = fs_utils_mkdir_p("/Volumes/DataMBP/new_folder/new_inner_folder", 0777);
if (res_void.error_code) { printf("This is not ok\n"); }
