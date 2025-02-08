/**
 * @file filesystem.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_FILESYSTEM_H
#define LIB_FILESYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <library/internal/module.h>

#define MAX_PATH_LENGTH          512
#define MAX_DIR_ENTRIES          512


// TODO: support utf8 encoding.
typedef struct allocator_t allocator_t;

typedef char dir_name_t[MAX_PATH_LENGTH];

typedef
struct dir_entries_t {
  dir_name_t dir_names[MAX_DIR_ENTRIES];
  uint32_t used;
} dir_entries_t;

LIBRARY_API
void
get_subdirectories(
  const char* directory,
  dir_entries_t* entries);

#ifdef __cplusplus
}
#endif

#endif