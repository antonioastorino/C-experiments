#include "fs_utils.h"
#include <errno.h>
#include <stdio.h>
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
    printf("Trying to create a folder\n");
    if (mkdir(dir_path, permission) == -1) {
      result = Err(_v, "Failed to create folder.");
    } else {
      printf("Folder successfully created");
      result = Ok(_v);
    }
  } else {
    result = Err(_v, "The folder already exists.");
  }

  // Restore the previous mask.
  umask(old_mask);
  return result;
}
