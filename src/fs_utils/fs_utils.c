#include "fs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

Result_void fs_utils_mkdir(const char *dir_path, mode_t permission) {
  Result_void result;
  void *_v;
  struct stat st = {0};
  // Save the current mode mask.
  mode_t old_mask = umask(0);
  // Unfilter all modes.
  mkdir("/tmp/mkdir-test", 0777);
  if (stat(dir_path, &st) == -1) {
    printf("Trying to create %s\n", dir_path);
    if (mkdir(dir_path, permission) == -1) {
      result = Err(_v, "Failed to create folder.", errno);
    } else {
      printf("Folder successfully created\n");
      result = Ok(_v);
    }
  } else {
    result = Err(_v, "The folder already exists.", errno);
  }

  // Restore the previous mask.
  umask(old_mask);
  return result;
}

Result_void fs_utils_mkdir_p(const char *dir_path, mode_t permission) {
  Result_void result;
  size_t path_length = strlen(dir_path);
  void *_v;
  printf("Received %s\n", dir_path);

  char partial_path[path_length + 1];
  size_t start_index = 0;
  if (dir_path[0] == '/') {
    partial_path[0] = dir_path[0];
    start_index = 1;
  }
  for (size_t i = start_index; i < path_length; i++) {
    if (dir_path[i] == '/') {
      // Terminate the partial string here.
      partial_path[i] = 0;
      result = fs_utils_mkdir((const char *)partial_path, permission);
      if ((result.error_code != EEXIST) && (result.error_code != 0)) {
        printf("Failed to create `%s`: %s\n", dir_path, result.err);
        return Err(_v, result.err, result.error_code);
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
      return Err(_v, result.err, result.error_code);
    }
  }
  printf("`%s` successfully created.\n", dir_path);
  return Ok(_v);
}

Result_void fs_utils_rmdir(const char *dir_path) {
  if (rmdir(dir_path)) {
    const char *error_msg;
    if (sscanf(error_msg, "Failed to delete `%s`.", (char *)dir_path)) {
      exit(1);
    }
    return Err(NULL, error_msg, errno);
  }
  return Ok(NULL);
}
