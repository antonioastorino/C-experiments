#include "fs_utils.h"
#include "logger.h"
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
// Private.
Result_void rm_r(FTS*, const char*);

bool fs_utils_does_exist(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        return false;
    } else {
        return true;
    }
}

/* ------------------------------------------ Folders ------------------------------------------- */
Result_void fs_utils_mkdir(const char* dir_path_p, mode_t permission) {
    Result_void result;
    // Save the current mode mask and reset the mask.
    mode_t old_mask = umask(0);
    if (!fs_utils_does_exist(dir_path_p)) {
        printf("Trying to create %s\n", dir_path_p);
        if (mkdir(dir_path_p, permission) == -1) {
            result = Err(NULL, "Failed to create folder.", errno);
        } else {
            printf("Folder successfully created\n");
            result = Ok(NULL);
        }
    } else {
        result = Err(NULL, "The folder already exists.", errno);
    }
    // Restore the previous mask.
    umask(old_mask);
    return result;
}

Result_void fs_utils_mkdir_p(const char* dir_path_p, mode_t permission) {
    Result_void result;
    size_t path_length = strlen(dir_path_p);
    printf("Trying to create `%s`\n", dir_path_p);
    if (fs_utils_does_exist(dir_path_p)) {
        printf("The folder already exists\n");
        return Ok(NULL);
    }
    char partial_path[path_length + 1];
    size_t start_index = 0;
    if (dir_path_p[0] == '/') {
        partial_path[0] = dir_path_p[0];
        start_index     = 1;
    }
    for (size_t i = start_index; i < path_length; i++) {
        if (dir_path_p[i] == '/') {
            // Terminate the partial string here.
            partial_path[i] = 0;
            printf("Trying to create `%s`\n", partial_path);
            // Check if this path exists or try to create it.
            if (!fs_utils_does_exist((const char*)partial_path)) {
                result = fs_utils_mkdir((const char*)partial_path, permission);
                RET_ON_ERR(result)
            }
        }
        // Append path chars to partial_path
        partial_path[i] = dir_path_p[i];
    }
    /*
    If the path terminated with a '/', then it was created. Otherwise, we need to make the last
    call.
    */
    if (dir_path_p[path_length - 1] != '/') {
        result = fs_utils_mkdir(dir_path_p, permission);
        RET_ON_ERR(result)
    }
    printf("`%s` successfully created.\n", dir_path_p);
    return Ok(NULL);
}

Result_void fs_utils_rmdir(const char* dir_path_p) {
    printf("Trying to remove `%s` folder\n", dir_path_p);
    if (rmdir(dir_path_p)) {
        char error_msg[100];
        if (sprintf(error_msg, "Failed to delete `%s`.", (char*)dir_path_p) < 0) {
            printf("Failed to concatenate string.\n");
        }
        return Err(NULL, error_msg, errno);
    }

    printf("`%s` folder successfully removed.\n", dir_path_p);
    return Ok(NULL);
}

/* ------------------------------------------- Files -------------------------------------------- */
Result_void fs_utils_rm(const char* file_path_p) {
    Result_void result;
    char* paths[] = {(char*)file_path_p, NULL};
    // Create the received path handle.
    FTS* fts_p = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (fts_p == NULL) { return Err(NULL, "Failed to initialize fts", errno); }
    /*
      Get next entry (could be file or directory). No need to check for ENOENT because we just
      found it.
      */
    FTSENT* dir_entry_p = fts_read(fts_p);
    if (dir_entry_p->fts_info & FTS_F) {
        // It is a file.
        if (unlink(file_path_p)) {
            printf("Removal failed with errno: %d\n", errno);
            result = Err(NULL, "Could not remove file.", errno);
        }
    } else {
        // It is a folder.
        result = rm_r(fts_p, file_path_p);
    }
    fts_close(fts_p);
    RET_ON_ERR(result)
    printf("`%s` successfully deleted.\n", file_path_p);
    return Ok(NULL);
}

/* ------------------------------------- Files and folders -------------------------------------- */
Result_void rm_r(FTS* fts_p, const char* dir_path_p) {
    Result_void result;
    /*
    Get next entry (could be file or directory). No need to check for ENOENT because we just found
    it.
    */
    FTSENT* dir_entry_p = fts_read(fts_p);
    if (dir_entry_p->fts_info & FTS_D) {
        // We are inside a directory - recurse;
        FTSENT* children = fts_children(fts_p, 0);
        printf("Folder `%s` found\n", dir_entry_p->fts_path);
        FTSENT* link = children;
        while (link != NULL) {
            printf("Found child: %s.\n", link->fts_name);
            char child_path[100];
            if (sprintf(child_path, "%s/%s", dir_path_p, link->fts_name) < 0) {
                return Err(NULL, "Failed to concatenate string.", errno);
            }
            printf("Trying: %s.\n", child_path);
            // Do your recursion thing.
            result = rm_r(fts_p, child_path);
            if (result.error_code) { return result; }
            link = link->fts_link;
        }
    }
    // The recursion is over. Delete what you found.
    if (dir_entry_p->fts_info & FTS_D) {
        printf("Trying to delete folder `%s`\n", dir_path_p);
        result = fs_utils_rmdir(dir_path_p);
        if (result.error_code) {
            return Err(NULL, "Could not remove directory.", result.error_code);
        }
    } else if (dir_entry_p->fts_info & FTS_F) {
        printf("Trying to delete file `%s`\n", dir_path_p);
        if (unlink(dir_path_p)) {
            printf("Removal failed with errno: %d\n", errno);
            return Err(NULL, "Could not remove file.", errno);
        }
    }
    return Ok(NULL);
}

Result_void fs_utils_rm_r(const char* dir_path_p) {
    Result_void result;
    printf("Trying to remove `%s` recursively \n", dir_path_p);
    char* paths[] = {(char*)dir_path_p, NULL};
    // Create the received path handle.
    FTS* fts_p = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (fts_p == NULL) {
        result = Err(NULL, "Failed to initialize fts", errno);
        fts_close(fts_p);
        return result;
    }
    // Start deleting recursively.
    result = rm_r(fts_p, dir_path_p);
    fts_close(fts_p);
    RET_ON_ERR(result)

    printf("`%s` successfully deleted.\n", dir_path_p);
    return Ok(NULL);
}
