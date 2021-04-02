#include "fs_utils.h"
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

bool fs_utils_does_exist(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        return false;
    } else {
        return true;
    }
}

Result_void fs_utils_mkdir(const char* dir_path_p, mode_t permission) {
    Result_void result;
    // Save the current mode mask and reset the mask.
    mode_t old_mask = umask(0);
    mkdir("/tmp/mkdir-test", 0777);
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

Result_void fs_utils_mkdir_p(const char* dir_path, mode_t permission) {
    Result_void result;
    size_t path_length = strlen(dir_path);
    printf("Received %s\n", dir_path);

    char partial_path[path_length + 1];
    size_t start_index = 0;
    if (dir_path[0] == '/') {
        partial_path[0] = dir_path[0];
        start_index     = 1;
    }
    for (size_t i = start_index; i < path_length; i++) {
        if (dir_path[i] == '/') {
            // Terminate the partial string here.
            partial_path[i] = 0;
            result          = fs_utils_mkdir((const char*)partial_path, permission);
            if ((result.error_code != EEXIST) && (result.error_code != 0)) {
                printf("Failed to create `%s`: %s\n", dir_path, result.err);
                return Err(NULL, result.err, result.error_code);
            }
        }
        // Append path chars to partial_path
        partial_path[i] = dir_path[i];
    }
    // If the path terminated with a '/', then it was created. Otherwise, we need
    // to make the last call.
    if (dir_path[path_length - 1] != '/') {
        result = fs_utils_mkdir(dir_path, permission);
        if ((result.error_code != EEXIST) && (result.error_code != 0)) {
            printf("Failed to create `%s`: %s\n", dir_path, result.err);
            return Err(NULL, result.err, result.error_code);
        }
    }
    printf("`%s` successfully created.\n", dir_path);
    return Ok(NULL);
}

Result_void fs_utils_rmdir(const char* dir_path) {
    if (rmdir(dir_path)) {
        char error_msg[100];
        if (sprintf(error_msg, "Failed to delete `%s`.", (char*)dir_path) < 0) { exit(1); }
        return Err(NULL, error_msg, errno);
    }
    return Ok(NULL);
}

Result_void recursive_fs_utils_rm_r(FTS* fts_p, const char* dir_path_p) {
    Result_void result;
    char* paths[] = {(char*)dir_path_p, NULL};
    // Get next entry (could be file or directory).
    FTSENT* dir_entry_p = fts_read(fts_p);

    if (dir_entry_p == NULL) {
        if (errno == 0) {
            // This entry is empty.
            printf("Maybe empty?");
            return Ok(NULL);
        } else {
            printf("Or maybe not");
            return Err(NULL, "Failed to read the currend dir_entry", errno);
        }
    }

    if (dir_entry_p->fts_info & FTS_D) {
        // We are inside a directory - recurse;
        FTSENT* children = fts_children(fts_p, 0);
        printf("Folder `%s` found\n", dir_entry_p->fts_path);
        FTSENT* link = children;
        while (link != NULL) {
            printf("Found child: %s.\n", link->fts_name);
            printf("Is folder: %d.\n", link->fts_info);
            char child_path[100];
            if (sprintf(child_path, "%s/%s", dir_path_p, link->fts_name) < 0) { exit(1); }
            printf("Trying: %s.\n", child_path);
            // Do your recursion thing.
            result = recursive_fs_utils_rm_r(fts_p, child_path);
            {
                if (result.error_code) { return result; }
            }
            link = link->fts_link;
        }
    } // The recursion is over. Delete what you found.
    sleep(4);
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
            return Err(NULL, "Could not remove file.", result.error_code);
        }
    }
    return Ok(NULL);
}

Result_void fs_utils_rm_r(const char* dir_path_p) {
    Result_void result;
    if (!fs_utils_does_exist(dir_path_p)) { return Err(NULL, "File or directory not found", 1); }
    char* paths[] = {(char*)dir_path_p, NULL};
    // Create the received path handle.
    FTS* fts_p = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
    if (fts_p == NULL) { return Err(NULL, "Failed to initialize fts", errno); }
    // Get next entry (could be file or directory).
    recursive_fs_utils_rm_r(fts_p, dir_path_p);
    fts_close(fts_p);

    printf("`%s` successfully deleted.\n", dir_path_p);
    return Ok(NULL);
}
