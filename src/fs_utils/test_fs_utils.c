#include "common.h"
#include "fs_utils.h"

#if TEST == 1
void test_fs_utils()
{
    PRINT_BANNER();
    Result_void_p res_void;
    Error err;
    char* path_string;
    char* content_char_p;
    String* content_string;

    PRINT_TEST_TITLE("mkdir - pass")
    path_string = "test/artifacts/test_folder_0";
    res_void    = fs_utils_mkdir(path_string, 0666);
    ASSERT_EQ((int)res_void.err.code, ERR_ALL_GOOD, "`fs_utils_mkdir` works fine.");

    PRINT_TEST_TITLE("mkdir - should fail")
    res_void = fs_utils_mkdir(path_string, 0666);
    ASSERT_EQ((int)res_void.err.code, ERR_FOLDER_EXISTS,
              "`fs_utils_mkdir` should fail if the folder exists.");

    PRINT_TEST_TITLE("mkdir -p - should fail")
    res_void = fs_utils_mkdir_p(path_string, 0666);
    ASSERT_EQ((int)res_void.err.code, ERR_FOLDER_EXISTS,
              "`fs_utils_mkdir` should fail if the folder exists.");

    PRINT_TEST_TITLE("mkdir -p - pass")
    path_string = "test/artifacts/test_folder_1/new_inner_folder/";
    res_void    = fs_utils_mkdir_p(path_string, 0777);
    ASSERT_EQ((int)res_void.err.code, ERR_ALL_GOOD,
              "`fs_utils_mkdir_p` works fine when the path ends with '/'.");

    PRINT_TEST_TITLE("mkdir -p - pass")
    path_string = "test/artifacts/test_folder_2/new_inner_folder";
    res_void    = fs_utils_mkdir_p(path_string, 0777);
    ASSERT_EQ((int)res_void.err.code, ERR_ALL_GOOD,
              "`fs_utils_mkdir_p` works fine when the path does not end with '/'.");

    PRINT_TEST_TITLE("rmdir - pass")
    path_string = "test/artifacts/empty-0";
    res_void    = fs_utils_rmdir(path_string);
    ASSERT_EQ((int)res_void.err.code, ERR_ALL_GOOD,
              "`fs_utils_rmdir` works fine when the folder is empty.");

    PRINT_TEST_TITLE("rmdir - fail")
    path_string = "test/artifacts/non-empty-0";
    err         = unwrap_err(fs_utils_rmdir(path_string));
    ASSERT_EQ((int)err.code, 66, "`fs_utils_rmdir` should fail if the folder is not empty.");

    PRINT_TEST_TITLE("rm -r - pass")
    path_string = "test/artifacts/empty";
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ((int)res_void.err.code, ERR_ALL_GOOD,
              "`fs_utils_rm_r` works fine when the folder is empty.");

    PRINT_TEST_TITLE("rm -r - pass")
    path_string = "test/artifacts/non-empty";
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ((int)res_void.err.code, ERR_ALL_GOOD,
              "`fs_utils_rm_r` works fine when the folder is NOT empty.");

    PRINT_TEST_TITLE("rm -r - fail")
    path_string = "test/artifacts/missing";
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ((int)res_void.err.code, ERR_NOT_FOUND,
              "`fs_utils_rm_r` should fail if the folder is missing.");

    PRINT_TEST_TITLE("rm -r - pass")
    path_string = "test/artifacts/delete_me.txt";
    res_void    = fs_utils_rm_r(path_string);
    ASSERT_EQ((int)res_void.err.code, ERR_ALL_GOOD, "`fs_utils_rm_r` should NOT fail on a file.");

    PRINT_TEST_TITLE("read to string - short text, use const String*for file path");
    path_string    = "test/assets/readme.txt";
    content_string = unwrap(fs_utils_read_to_string(path_string));
    ASSERT_EQ(content_string->str, "This is a very good string!", "File read correctly.");
    String_destroy(content_string);

    PRINT_TEST_TITLE("read to string - short text, use const char * for file path");
    content_string = unwrap(fs_utils_read_to_string("test/assets/readme.txt"));
    ASSERT_EQ(content_string->str, "This is a very good string!", "File read correctly.");
    String_destroy(content_string);

    PRINT_TEST_TITLE("read to string - long text");
    path_string    = "test/assets/readme-long.txt";
    content_string = unwrap(fs_utils_read_to_string(path_string));
    ASSERT_EQ(content_string->length, 16599, "Read string size matches.");
    String_destroy(content_string);

    PRINT_TEST_TITLE("append to initially missing file");
    path_string    = "test/artifacts/new-file.txt";
    content_char_p = "this is new\n";
    fs_utils_append(path_string, content_char_p);
    fs_utils_append(path_string, content_char_p);
    content_string = unwrap(fs_utils_read_to_string(path_string));
    ASSERT_EQ(content_string->str, "this is new\nthis is new\n",
              "File created and modified correctly");
    String_destroy(content_string);

    PRINT_TEST_TITLE("create with content string");
    path_string    = "test/artifacts/new-file-2.txt";
    content_char_p = "this is new\n";
    // First time - create.
    fs_utils_create_with_content(path_string, content_char_p);
    // Second time - overwrite.
    fs_utils_create_with_content(path_string, content_char_p);
    content_string = unwrap(fs_utils_read_to_string(path_string));
    ASSERT_EQ(content_string->str, "this is new\n", "File created and modified correctly");
    String_destroy(content_string);
}
#endif
