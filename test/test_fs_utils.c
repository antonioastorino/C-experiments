#include "fs_utils.h"
#include <sys/stat.h>

void test_fs_utils()
{
    size_t counter = 0;
    PRINT_BANNER("Testing fs_utils");
    Result res_void;
    Error err;
    String* path_string;

    PRINT_TEST_TITLE("mkdir - pass")
    path_string = unwrap_String_p(String_new("test/artifacts/test_folder_0"));
    res_void    = fs_utils_mkdir(path_string, 0666);
    ASSERT_EQ(unwrap_void_p(res_void), NULL, "`fs_utils_mkdir` works fine.");

    PRINT_TEST_TITLE("mkdir - should fail")
    res_void = fs_utils_mkdir(path_string, 0666);
    ASSERT_EQ(unwrap_err(res_void).code, 2, "`fs_utils_mkdir` should fail if the folder exists.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("mkdir -p - pass")
    path_string = unwrap_String_p(String_new("test/artifacts/test_folder_1/new_inner_folder/"));
    res_void    = fs_utils_mkdir_p(path_string, 0777);
    ASSERT_EQ(unwrap_void_p(res_void), NULL,
              "`fs_utils_mkdir_p` works fine when the path ends with '/'.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("mkdir -p - pass")
    path_string = unwrap_String_p(String_new("test/artifacts/test_folder_2/new_inner_folder"));
    res_void    = fs_utils_mkdir_p(path_string, 0777);
    ASSERT_EQ(unwrap_void_p(res_void), NULL,
              "`fs_utils_mkdir_p` works fine when the path does not end with '/'.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("rmdir - pass")
    path_string = unwrap_String_p(String_new("test/artifacts/empty-0"));
    res_void    = fs_utils_rmdir(path_string);
    ASSERT_EQ(unwrap_void_p(res_void), NULL,
              "`fs_utils_rmdir` works fine when the folder is empty.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("rmdir - fail")
    path_string = unwrap_String_p(String_new("test/artifacts/non-empty-0"));
    err         = unwrap_err(fs_utils_rmdir(path_string));
    printf("%s, errno: %d\n", err.message, err.code);
    ASSERT_EQ(err.code, 66, "`fs_utils_rmdir` should fail if the folder is not empty.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("rm -r - pass")
    path_string = unwrap_String_p(String_new("test/artifacts/empty"));
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ(unwrap_void_p(res_void), NULL,
              "`fs_utils_rm_r` works fine when the folder is empty.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("rm -r - pass")
    path_string = unwrap_String_p(String_new("test/artifacts/non-empty"));
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ(unwrap_void_p(res_void), NULL,
              "`fs_utils_rm_r` works fine when the folder is NOT empty.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("rm -r - fail")
    path_string = unwrap_String_p(String_new("test/artifacts/missing"));
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ(unwrap_err(res_void).code, 2,
              "`fs_utils_rm_r` should fail if the folder is missing.");
    String_destroy(path_string);

    PRINT_TEST_TITLE("rm -r - pass")
    path_string = unwrap_String_p(String_new("test/artifacts/deleteme.txt"));
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ(unwrap_void_p(res_void), NULL, "`fs_utils_rm_r` should NOT fail on a file.");
    String_destroy(path_string);
}