#include "fs_utils.h"
#include "logger.h"
#include "mem.h"
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

Result_void_p recursive_rm_r(FTS*, const char*);

bool fs_utils_does_exist(const char* p_path)
{
    struct stat st = {0};
    if (stat(p_path, &st) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/* ------------------------------------------ Folders ------------------------------------------- */
Result_void_p fs_utils_mkdir(const char* dir_path_char_p, mode_t permission)
{
    Result_void_p result = Ok(NULL);
    // Save the current mode mask and reset the mask.
    mode_t old_mask = umask(0);
    if (!fs_utils_does_exist(dir_path_char_p))
    {
        LOG_TRACE("Trying to create %s", dir_path_char_p);
        if (mkdir(dir_path_char_p, permission) == -1)
        {
            LOG_ERROR("mkdir returned with error code %d", errno);
            result = Err(NULL, "Failed to create folder.", ERR_GENERIC);
        }
        else
        {
            LOG_TRACE("Folder successfully created.", NULL);
            result = Ok(NULL);
        }
    }
    else
    {
        result = Err(NULL, "The folder already exists.", ERR_FOLDER_EXISTS);
    }
    // Restore the previous mask.
    umask(old_mask);
    return result;
}

Result_void_p fs_utils_mkdir_p(const char* dir_path_char_p, mode_t permission)
{
    Result_void_p result = Ok(NULL);

    size_t path_length = strlen(dir_path_char_p);
    LOG_TRACE("Trying to create `%s`", dir_path_char_p);
    if (fs_utils_does_exist(dir_path_char_p))
    {
        return Err(NULL, "The folder already exists.", ERR_FOLDER_EXISTS);
    }
    char partial_path[path_length + 1];
    size_t start_index = 0;
    if (dir_path_char_p[0] == '/')
    {
        partial_path[0] = '/';
        // Skip the fist folder found (root folder) in the path - it's an absolute path.
        start_index = 1;
    }
    for (size_t i = start_index; i < path_length; i++)
    {
        if (dir_path_char_p[i] == '/')
        {
            // Terminate the partial string here.
            partial_path[i] = 0;
            LOG_TRACE("Trying to create `%s`.", partial_path);

            // Check if this path exists or try to create it.
            if (!fs_utils_does_exist(partial_path))
            {
                result = fs_utils_mkdir(partial_path, permission);
                RETURN_ON_ERROR(NULL, result)
            }
        }
        // Append path chars to partial_path
        partial_path[i] = dir_path_char_p[i];
    }
    /*
    If the path terminated with a '/', then it was created. Otherwise, we need to make the last
    call.
    */
    if (dir_path_char_p[path_length - 1] != '/')
    {
        result = fs_utils_mkdir(dir_path_char_p, permission);
        RETURN_ON_ERROR(NULL, result)
    }
    LOG_TRACE("`%s` successfully created.", dir_path_char_p);
    return Ok(NULL);
}

Result_void_p fs_utils_rmdir(const char* dir_path_char_p)
{
    LOG_INFO("Trying to remove `%s` folder.", dir_path_char_p);
    if (!fs_utils_does_exist(dir_path_char_p))
    {
        return Err(NULL, "Folder not found.", ERR_NOT_FOUND);
    }
    if (rmdir(dir_path_char_p))
    {
        LOG_ERROR("Failed to delete `%s`.", dir_path_char_p)

        return Err(NULL, "Failed to delete folder.", errno);
    }

    LOG_INFO("`%s` folder successfully removed.", dir_path_char_p);
    return Ok(NULL);
}

/* ------------------------------------------- Files -------------------------------------------- */
Result_void_p fs_utils_rm_from_path_as_char_p(const char* file_path_char_p)
{
    Result_void_p result = Ok(NULL);
    char* paths[2]       = {(char*)file_path_char_p, NULL};
    // Create the received path handle.
    FTS* fts_p = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (fts_p == NULL)
    {
        return Err(NULL, "Failed to initialize fts", errno);
    }
    /*
      Get next entry (could be file or directory). No need to check for ENOENT because we just
      found it.
      */
    FTSENT* dir_entry_p = fts_read(fts_p);
    if (dir_entry_p->fts_info & FTS_F)
    {
        // It is a file.
        if (unlink(file_path_char_p))
        {
            LOG_TRACE("Removal failed with errno: %d.", errno);
            result = Err(NULL, "Could not remove file.", errno);
        }
    }
    else
    {
        // It is a folder.
        result = fs_utils_rmdir(file_path_char_p);
    }
    fts_close(fts_p);
    RETURN_ON_ERROR(NULL, result)
    LOG_TRACE("`%s` successfully deleted.", file_path_char_p);
    return result;
}

Result_void_p fs_utils_append(const char* file_path_char_p, const char* new_content_char_p)
{
    FILE* file_handler = fopen(file_path_char_p, "a");
    if (!file_handler)
    {
        return Err(NULL, "Failed to open file.", ERR_NULL);
    }
    fprintf(file_handler, "%s", new_content_char_p);
    fclose(file_handler);
    return Ok(NULL);
}

Result_void_p fs_utils_create_with_content(const char* file_path_char_p,
                                           const char* new_content_char_p)
{
    FILE* file_handler = fopen(file_path_char_p, "w");
    if (!file_handler)
    {
        return Err(NULL, "Failed to open file.", ERR_NULL);
    }
    fprintf(file_handler, "%s", new_content_char_p);
    fclose(file_handler);
    return Ok(NULL);
}

Result_String_p _fs_utils_read_to_string(const char* file_path_char_p)
{
    String* ret_string_p;
    FILE* file_handler = fopen(file_path_char_p, "r");
    if (!file_handler)
    {
        LOG_ERROR("Failed to open `%s`", file_path_char_p);
        return Err(ret_string_p, "Failed to open file.", ERR_NULL);
    }
    int c;
    size_t chars_read = 0;
    size_t size       = 4096;
    char* buf         = MALLOC(size);
    if (buf == NULL)
    {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    while ((c = getc(file_handler)) != EOF)
    {
        if (chars_read >= size - 1)
        {
            /* time to make it bigger */
            size = (size_t)(size * 1.5);
            buf  = REALLOCF(buf, size);
            if (buf == NULL)
            {
                fprintf(stderr, "out of memory\n");
                exit(1);
            }
        }
        buf[chars_read++] = c;
    }
    buf[chars_read++] = '\0';
    fclose(file_handler);
    ret_string_p = String_new(buf);
    FREE(buf);
    buf = NULL;
    return Ok(ret_string_p);
}

/* ------------------------------------- Files and folders -------------------------------------- */
Result_void_p recursive_rm_r(FTS* fts_p, const char* dir_path_char_p)
{
    Result_void_p result = Ok(NULL);
    /*
    Get next entry (could be file or directory). No need to check for ENOENT because we just found
    it.
    */
    FTSENT* dir_entry_p = fts_read(fts_p);
    if (dir_entry_p->fts_info & FTS_D)
    {
        // We are inside a directory - recurse;
        FTSENT* children = fts_children(fts_p, 0);
        LOG_TRACE("Folder `%s` found", dir_entry_p->fts_path);
        FTSENT* link = children;
        while (link != NULL)
        {
            LOG_TRACE("Found child: %s.", link->fts_name);
            // TODO: String_join()
            String* child_path_string = String_new("%s/%s", dir_path_char_p, link->fts_name);

            LOG_TRACE("Trying: %s.", child_path_string->str);
            // Do your recursion thing.
            result = recursive_rm_r(fts_p, child_path_string->str);
            String_destroy(child_path_string);
            RETURN_ON_ERROR(NULL, result)
            // Go to the next entry.
            link = link->fts_link;
        }
    }
    // The recursion is over. Delete what you found.
    if (dir_entry_p->fts_info & FTS_D)
    {
        LOG_TRACE("Trying to delete folder `%s`", dir_path_char_p);
        result = fs_utils_rmdir(dir_path_char_p);
        RETURN_ON_ERROR(NULL, result)
    }
    else if (dir_entry_p->fts_info & FTS_F)
    {
        LOG_TRACE("Trying to delete file `%s`", dir_path_char_p);
        if (unlink(dir_path_char_p))
        {
            LOG_ERROR("Removal failed with errno: %d.", errno);
            return Err(NULL, "Could not remove file.", errno);
        }
    }
    return result;
}

Result_void_p fs_utils_rm_r(const char* p_string_dir_path)
{
    Result_void_p result = Ok(NULL);
    LOG_INFO("Trying to remove `%s` recursively.", p_string_dir_path);
    if (!fs_utils_does_exist(p_string_dir_path))
    {
        return Err(NULL, "Folder not found", ERR_NOT_FOUND);
    }
    char* paths[] = {(char*)p_string_dir_path, NULL};
    // Create the received path handle.
    FTS* fts_p = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (fts_p == NULL)
    {
        result = Err(NULL, "Failed to initialize fts", errno);
        fts_close(fts_p);
        return result;
    }
    // Start deleting recursively.
    result = recursive_rm_r(fts_p, p_string_dir_path);
    fts_close(fts_p);
    RETURN_ON_ERROR(NULL, result)

    LOG_INFO("`%s` successfully deleted.", p_string_dir_path);
    return result;
}

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
