#include "fs_utils.h"
const char* folder1 = "test/artifacts/test_folder_1/new_inner_folder/";
const char* folder2 = "test/artifacts/test_folder_2/new_inner_folder";

PRINT_BANNER("Testing fs_utils");
Result_void res_void;

res_void = fs_utils_mkdir_p(folder1, 0777);
if (res_void.error_code) { printf("This is not ok\n"); }

res_void = fs_utils_mkdir_p(folder2, 0777);
if (res_void.error_code) { printf("This is not ok\n"); }
