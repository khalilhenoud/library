/**
 * @file os.h
 * @author khalilhenoud@gmail.com
 * @brief wrapper to all non-window specific os functions
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef LIBRARY_OS_H
#define LIBRARY_OS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <library/internal/module.h>

#define INVALID_HANDLE ((uintptr_t)(-1))
#define LIBRARY_ERROR_NO_MORE_FILES       18L


typedef
struct cursor_pos_t {
  int64_t x;
  int64_t y;
} cursor_pos_t;

typedef
uintptr_t file_handle_t;

typedef
struct file_find_data_t {
  const char *name;
  uint64_t file_attributes;
} file_find_data_t;

typedef
enum file_attributes_t {
  FILE_IS_DIRECTORY            = 0x00000010
} file_attributes_t;

////////////////////////////////////////////////////////////////////////////////
LIBRARY_API
int32_t
get_screen_width();

LIBRARY_API
int32_t
get_screen_height();

////////////////////////////////////////////////////////////////////////////////
LIBRARY_API
void
set_periodic_timers_resolution(uint32_t ms);

LIBRARY_API
void
end_periodic_timers_resolution(uint32_t ms);

LIBRARY_API
uint64_t
time_get_time();

LIBRARY_API
int32_t
get_performance_frequency(uint64_t *frequency);

LIBRARY_API
int32_t
get_performance_counter(uint64_t *counter);

////////////////////////////////////////////////////////////////////////////////
LIBRARY_API
int32_t
get_keyboard_state(uint8_t key_states[256]);

LIBRARY_API
int16_t
get_async_key_state(int32_t key);

LIBRARY_API
int32_t
show_cursor(int32_t show);

LIBRARY_API
int32_t
get_cursor_position(cursor_pos_t *position);

LIBRARY_API
int32_t
set_cursor_position(int32_t x, int32_t y);

////////////////////////////////////////////////////////////////////////////////
LIBRARY_API
void
sleep(uint64_t ms);

////////////////////////////////////////////////////////////////////////////////
LIBRARY_API
file_handle_t
find_first_file(const char *file, file_find_data_t *file_data);

LIBRARY_API
int32_t
find_next_file(file_handle_t handle, file_find_data_t *file_data);

LIBRARY_API
int32_t
find_close(file_handle_t handle);

LIBRARY_API
uint64_t
get_last_error();

#ifdef __cplusplus
}
#endif

#endif