#include "fs_utils.h"
#include <sys/stat.h>

void test_fs_utils() {
    const char* folder_0 = "test/artifacts/test_folder_0";
    const char* folder_1 = "test/artifacts/test_folder_1/new_inner_folder/";
    const char* folder_2 = "test/artifacts/test_folder_2/new_inner_folder";
    size_t counter       = 0;
    PRINT_BANNER("Testing fs_utils");
    Result_void res_void;

    PRINT_TEST_TITLE("mkdir - pass")
    res_void = fs_utils_mkdir(folder_0, 0666);
    ASSERT_EQ(res_void.error_code, 0, "`fs_utils_mkdir` works fine.");

    PRINT_TEST_TITLE("mkdir - should fail")
    res_void = fs_utils_mkdir(folder_0, 0666);
    ASSERT_EQ(res_void.error_code, 2, "`fs_utils_mkdir` should fail if the folder exists.");

    PRINT_TEST_TITLE("mkdir - ")
    res_void = fs_utils_mkdir_p(folder_1, 0777);
    ASSERT_EQ(res_void.error_code, 0, "`fs_utils_mkdir_p` works fine when the path ends with '/'.");

    res_void = fs_utils_mkdir_p(folder_2, 0777);
    ASSERT_EQ(res_void.error_code, 0,
              "`fs_utils_mkdir_p` works fine when the path does not end with '/'.");

    res_void = fs_utils_rmdir("test/artifacts/empty-0");
    ASSERT_EQ(res_void.error_code, 0, "`fs_utils_rmdir` works fine when the folder is empty.");

    res_void = fs_utils_rmdir("test/artifacts/non-empty-0");
    printf("%s, errno: %d\n", res_void.err, res_void.error_code);
    ASSERT_EQ(res_void.error_code, 66, "`fs_utils_rmdir` should fail if the folder is not empty.");

    res_void = fs_utils_rm_r("test/artifacts/empty");
    ASSERT_EQ(res_void.error_code, 0, "`fs_utils_rm_r` works fine when the folder is empty.");

    res_void = fs_utils_rm_r("test/artifacts/non-empty");
    ASSERT_EQ(res_void.error_code, 0, "`fs_utils_rm_r` works fine when the folder is NOT empty.");

    res_void = fs_utils_rm_r("test/artifacts/missing");
    ASSERT_EQ(res_void.error_code, 2, "`fs_utils_rm_r` should fail if the folder is missing.");

    res_void = fs_utils_rm_r("test/artifacts/deleteme.txt");
    ASSERT_EQ(res_void.error_code, 0, "`fs_utils_rm_r` should NOT fail on a file.");
}