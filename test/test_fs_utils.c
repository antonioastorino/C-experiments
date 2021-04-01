#include "fs_utils.h"
#include <sys/stat.h>
const char *folder_0 = "test/artifacts/test_folder_0";
const char *folder_1 = "test/artifacts/test_folder_1/new_inner_folder/";
const char *folder_2 = "test/artifacts/test_folder_2/new_inner_folder";

PRINT_BANNER("Testing fs_utils");
Result_void res_void;
struct stat st = {0};

res_void = fs_utils_mkdir(folder_0, 0666);
ASSERT_EQ(stat(folder_0, &st), 0, "`fs_utils_mkdir` works fine");

res_void = fs_utils_mkdir_p(folder_1, 0777);
ASSERT_EQ(stat(folder_1, &st), 0,
          "`fs_utils_mkdir_p` works fine when the path ends with '/'");

res_void = fs_utils_mkdir_p(folder_2, 0777);
ASSERT_EQ(stat(folder_2, &st), 0,
          "`fs_utils_mkdir_p` works fine when the path does not end with '/'");

res_void = fs_utils_rmdir(folder_2);
ASSERT_EQ(stat(folder_2, &st), -1,
          "`fs_utils_rmdir` works fine when the folder is empty");
