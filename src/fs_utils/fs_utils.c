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
Result_void recursive_rm_r(FTS*, String*);

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
Result_void fs_utils_mkdir(String* p_string_dir_path, mode_t permission)
{
    Result_void result;
    // Save the current mode mask and reset the mask.
    mode_t old_mask = umask(0);
    if (!fs_utils_does_exist(p_string_dir_path))
    {
        LOG(TRACE, "Trying to create %s", p_string_dir_path->str);
        if (mkdir(p_string_dir_path->str, permission) == -1)
        {
            result = Err(NULL, "Failed to create folder.", errno);
        }
        else
        {
            LOG(TRACE, "Folder successfully created.", NULL);
            result = Ok(NULL);
        }
    }
    else
    {
        result = Err(NULL, "The folder already exists.", errno);
    }
    // Restore the previous mask.
    umask(old_mask);
    return result;
}

Result_void fs_utils_mkdir_p(String* p_string_dir_path, mode_t permission)
{
    Result_void result;
    size_t path_length = p_string_dir_path->length;
    LOG(TRACE, "Trying to create `%s`\n", p_string_dir_path->str);
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
            String string_partial_path = String_new(partial_path);
            if (!fs_utils_does_exist(&string_partial_path))
            {
                result = fs_utils_mkdir(&string_partial_path, permission);
                RET_ON_ERR(result)
            }
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
    LOG(TRACE, "`%s` successfully created.", p_string_dir_path->str, NULL);
    return Ok(NULL);
}

Result_void fs_utils_rmdir(String* p_string_dir_path)
{
    LOG(INFO, "Trying to remove `%s` folder.", p_string_dir_path->str);
    if (rmdir(p_string_dir_path->str))
    {
        String error_message = String_new("Failed to delete `%s`.", p_string_dir_path);
        return Err(NULL, error_message.str, errno);
    }

    LOG(INFO, "`%s` folder successfully removed.", p_string_dir_path->str);
    return Ok(NULL);
}

/* ------------------------------------------- Files -------------------------------------------- */
Result_void fs_utils_rm(String* p_string_file_path)
{
    Result_void result;
    char* paths[] = {p_string_file_path->str, NULL};
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
        if (unlink(p_string_file_path->str))
        {
            LOG(TRACE, "Removal failed with errno: %d\n", errno);
            result = Err(NULL, "Could not remove file.", errno);
        }
    }
    else
    {
        // It is a folder.
        result = fs_utils_rmdir(p_string_file_path);
    }
    fts_close(fts_p);
    RET_ON_ERR(result)
    LOG(TRACE, "`%s` successfully deleted.\n", p_string_file_path->str);
    return Ok(NULL);
}

/* ------------------------------------- Files and folders -------------------------------------- */
Result_void recursive_rm_r(FTS* fts_p, String* p_string_dir_path)
{
    Result_void result;
    /*
    Get next entry (could be file or directory). No need to check for ENOENT because we just found
    it.
    */
    FTSENT* dir_entry_p = fts_read(fts_p);
    if (dir_entry_p->fts_info & FTS_D)
    {
        // We are inside a directory - recurse;
        FTSENT* children = fts_children(fts_p, 0);
        LOG(TRACE, "Folder `%s` found\n", dir_entry_p->fts_path);
        FTSENT* link = children;
        while (link != NULL)
        {
            LOG(TRACE, "Found child: %s.\n", link->fts_name);
            // TODO: String_join()
            String child_path_string = String_new("%s/%s", p_string_dir_path->str, link->fts_name);
            LOG(TRACE, "Trying: %s.\n", child_path_string.str);
            // Do your recursion thing.
            result = recursive_rm_r(fts_p, &child_path_string);
            if (result.error_code)
            {
                return result;
            }
            link = link->fts_link;
        }
    }
    // The recursion is over. Delete what you found.
    if (dir_entry_p->fts_info & FTS_D)
    {
        LOG(TRACE, "Trying to delete folder `%s`", p_string_dir_path->str);
        result = fs_utils_rmdir(p_string_dir_path);
        if (result.error_code)
        {
            // TODO: Log error.
            return Err(NULL, "Could not remove directory.", result.error_code);
        }
    }
    else if (dir_entry_p->fts_info & FTS_F)
    {
        LOG(TRACE, "Trying to delete file `%s`", p_string_dir_path->str);
        if (unlink(p_string_dir_path->str))
        {
            LOG(ERROR, "Removal failed with errno: %d.", errno);
            return Err(NULL, "Could not remove file.", errno);
        }
    }
    return Ok(NULL);
}

Result_void fs_utils_rm_r(String* p_string_dir_path)
{
    Result_void result;
    LOG(INFO, "Trying to remove `%s` recursively \n", p_string_dir_path->str);
    char* paths[] = {(char*)p_string_dir_path->str, NULL};
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
    RET_ON_ERR(result)

    LOG(INFO, "`%s` successfully deleted.\n", p_string_dir_path->str);
    return Ok(NULL);
}
