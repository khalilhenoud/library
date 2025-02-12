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
#include <windows.h>
#include <assert.h>
#include <library/filesystem/filesystem.h>


void
get_subdirectories(
  const char* directory,
  dir_entries_t* entries)
{
  assert(directory && entries);

  {
    WIN32_FIND_DATA ffd;
    HANDLE hFind;
    DWORD dwError = 0;
    entries->used = 0;

    hFind = FindFirstFile(directory, &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
      assert(0 && "get_subdirectories failed on first attempt!");

    // get all sub directories names.
    do {
        if (
          ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
          strcmp(ffd.cFileName, ".") && 
          strcmp(ffd.cFileName, "..")) {
          memset(
            entries->dir_names[entries->used], 
            0, 
            sizeof(entries->dir_names[entries->used]));

          memcpy(
            entries->dir_names[entries->used], 
            ffd.cFileName, 
            strlen(ffd.cFileName));

          entries->used++;
        }
    } while (FindNextFile(hFind, &ffd) != 0);
  
    dwError = GetLastError();
    assert(
      dwError == ERROR_NO_MORE_FILES && 
      "unknown error in get_subdirectories!");

    FindClose(hFind);
  }
}