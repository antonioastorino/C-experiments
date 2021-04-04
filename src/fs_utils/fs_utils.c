#include "fs_utils.h"
#include "logger.h"
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
// Private.
Result recursive_rm_r(FTS*, String*);

bool fs_utils_does_exist(String* p_path)
{
    struct stat st = {0};
    if (stat(p_path->str, &st) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/* ------------------------------------------ Folders ------------------------------------------- */
Result fs_utils_mkdir(String* p_string_dir_path, mode_t permission)
{
    Result result;
    // Save the current mode mask and reset the mask.
    mode_t old_mask = umask(0);
    if (!fs_utils_does_exist(p_string_dir_path))
    {
        LOG(TRACE, "Trying to create %s", p_string_dir_path->str);
        if (mkdir(p_string_dir_path->str, permission) == -1)
        {
            result = Err("Failed to create folder.", errno);
        }
        else
        {
            LOG(TRACE, "Folder successfully created.", NULL);
            result = Ok(NULL);
        }
    }
    else
    {
        result = Err("The folder already exists.", errno);
    }
    // Restore the previous mask.
    umask(old_mask);
    return result;
}

Result fs_utils_mkdir_p(String* p_string_dir_path, mode_t permission)
{
    Result result;
    size_t path_length = p_string_dir_path->length;
    LOG(TRACE, "Trying to create `%s`", p_string_dir_path->str);
    if (fs_utils_does_exist(p_string_dir_path))
    {
        LOG(TRACE, "The folder already exists.", NULL);
        return Ok(NULL);
    }
    char partial_path[path_length + 1];
    size_t start_index = 0;
    if (String_starts_with(p_string_dir_path, "/"))
    {
        partial_path[0] = '/';
        // Skip the fist folder found (root folder) in the path - it's an absolute path.
        start_index = 1;
    }
    for (size_t i = start_index; i < path_length; i++)
    {
        if (p_string_dir_path->str[i] == '/')
        {
            // Terminate the partial string here.
            partial_path[i] = 0;
            LOG(TRACE, "Trying to create `%s`.", partial_path);

            // Check if this path exists or try to create it.
            String* string_partial_path = unwrap_String_p(String_new(partial_path));
            if (!fs_utils_does_exist(string_partial_path))
            {
                result = fs_utils_mkdir(string_partial_path, permission);
                RET_ON_ERR(result)
            }
            String_destroy(string_partial_path);
        }
        // Append path chars to partial_path
        partial_path[i] = p_string_dir_path->str[i];
    }
    /*
    If the path terminated with a '/', then it was created. Otherwise, we need to make the last
    call.
    */
    if (p_string_dir_path->str[path_length - 1] != '/')
    {
        result = fs_utils_mkdir(p_string_dir_path, permission);
        RET_ON_ERR(result)
    }
    LOG(TRACE, "`%s` successfully created.", p_string_dir_path->str);
    return Ok(NULL);
}

Result fs_utils_rmdir(String* p_string_dir_path)
{
    LOG(INFO, "Trying to remove `%s` folder.", p_string_dir_path->str);
    if (rmdir(p_string_dir_path->str))
    {
        LOG(ERROR, "Failed to delete `%s`.", p_string_dir_path->str)
        String* error_message
            = unwrap_String_p(String_new("Failed to delete `%s`.", p_string_dir_path->str));
        String_destroy(error_message);
        return Err(error_message->str, errno);
    }

    LOG(INFO, "`%s` folder successfully removed.", p_string_dir_path->str);
    return Ok(NULL);
}

/* ------------------------------------------- Files -------------------------------------------- */
Result fs_utils_rm(String* p_string_file_path)
{
    Result result;
    char* paths[] = {p_string_file_path->str, NULL};
    // Create the received path handle.
    FTS* fts_p = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (fts_p == NULL)
    {
        return Err("Failed to initialize fts", errno);
    }
    /*
      Get next entry (could be file or directory). No need to check for ENOENT because we just
      found it.
      */
    FTSENT* dir_entry_p = fts_read(fts_p);
    if (dir_entry_p->fts_info & FTS_F)
    {
        // It is a file.
        if (unlink(p_string_file_path->str))
        {
            LOG(TRACE, "Removal failed with errno: %d.", errno);
            result = Err("Could not remove file.", errno);
        }
    }
    else
    {
        // It is a folder.
        result = fs_utils_rmdir(p_string_file_path);
    }
    fts_close(fts_p);
    String_destroy(p_string_file_path);
    RET_ON_ERR(result)
    LOG(TRACE, "`%s` successfully deleted.", p_string_file_path->str);
    return Ok(NULL);
}

/* ------------------------------------- Files and folders -------------------------------------- */
Result recursive_rm_r(FTS* fts_p, String* p_string_dir_path)
{
    Result result;
    /*
    Get next entry (could be file or directory). No need to check for ENOENT because we just found
    it.
    */
    FTSENT* dir_entry_p = fts_read(fts_p);
    if (dir_entry_p->fts_info & FTS_D)
    {
        // We are inside a directory - recurse;
        FTSENT* children = fts_children(fts_p, 0);
        LOG(TRACE, "Folder `%s` found", dir_entry_p->fts_path);
        FTSENT* link = children;
        while (link != NULL)
        {
            LOG(TRACE, "Found child: %s.", link->fts_name);
            // TODO: String_join()
            String* child_path_string
                = unwrap_String_p(String_new("%s/%s", p_string_dir_path->str, link->fts_name));

            LOG(TRACE, "Trying: %s.", child_path_string->str);
            // Do your recursion thing.
            result = recursive_rm_r(fts_p, child_path_string);
            String_destroy(child_path_string);
            RET_ON_ERR(result)
            // Go to the next entry.
            link = link->fts_link;
        }
    }
    // The recursion is over. Delete what you found.
    if (dir_entry_p->fts_info & FTS_D)
    {
        LOG(TRACE, "Trying to delete folder `%s`", p_string_dir_path->str);
        result = fs_utils_rmdir(p_string_dir_path);
        RET_ON_ERR(result)
    }
    else if (dir_entry_p->fts_info & FTS_F)
    {
        LOG(TRACE, "Trying to delete file `%s`", p_string_dir_path->str);
        if (unlink(p_string_dir_path->str))
        {
            LOG(ERROR, "Removal failed with errno: %d.", errno);
            return Err("Could not remove file.", errno);
        }
    }
    return Ok(NULL);
}

Result fs_utils_rm_r(String* p_string_dir_path)
{
    Result result;
    LOG(INFO, "Trying to remove `%s` recursively.", p_string_dir_path->str);
    char* paths[] = {(char*)p_string_dir_path->str, NULL};
    // Create the received path handle.
    FTS* fts_p = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (fts_p == NULL)
    {
        result = Err("Failed to initialize fts", errno);
        fts_close(fts_p);
        return result;
    }
    // Start deleting recursively.
    result = recursive_rm_r(fts_p, p_string_dir_path);
    fts_close(fts_p);
    RET_ON_ERR(result)

    LOG(INFO, "`%s` successfully deleted.", p_string_dir_path->str);
    return Ok(NULL);
}
