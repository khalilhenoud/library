/**
 * @file filesystem.impl
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-01-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <assert.h>
#include <library/os/os.h>
#include <library/filesystem/filesystem.h>


void
get_subdirectories(
  const char *directory,
  dir_entries_t *entries)
{
  assert(directory && entries);

  {
    file_find_data_t ffd;
    file_handle_t hFind;
    uint64_t error = 0;
    entries->used = 0;

    hFind = find_first_file(directory, &ffd);

    if (hFind == INVALID_HANDLE)
      assert(0 && "get_subdirectories failed on first attempt!");

    // get all sub directories names.
    do {
        if (
          ffd.file_attributes & FILE_IS_DIRECTORY &&
          strcmp(ffd.name, ".") && 
          strcmp(ffd.name, "..")) {
          memset(
            entries->dir_names[entries->used], 
            0, 
            sizeof(entries->dir_names[entries->used]));

          memcpy(
            entries->dir_names[entries->used], 
            ffd.name, 
            strlen(ffd.name));

          entries->used++;
        }
    } while (find_next_file(hFind, &ffd) != 0);
  
    error = get_last_error();
    assert(
      error == LIBRARY_ERROR_NO_MORE_FILES && 
      "unknown error in get_subdirectories!");

    find_close(hFind);
  }
}