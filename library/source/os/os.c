/**
 * @file os.c
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2025-11-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
// TODO: Implement static assert for C using negative indices array.
#endif
#include <library/os/os.h>


int32_t
get_screen_width()
{
  return GetSystemMetrics(SM_CXFULLSCREEN);
}

int32_t
get_screen_height()
{
  return GetSystemMetrics(SM_CYFULLSCREEN);
}

////////////////////////////////////////////////////////////////////////////////
void
set_periodic_timers_resolution(uint32_t ms)
{
  timeBeginPeriod(ms);
}

void
end_periodic_timers_resolution(uint32_t ms)
{
  timeEndPeriod(ms);
}

uint64_t
time_get_time()
{
  return timeGetTime();
}

int32_t
get_performance_frequency(uint64_t *frequency)
{
  return QueryPerformanceFrequency((LARGE_INTEGER*)frequency);
}

int32_t
get_performance_counter(uint64_t *counter)
{
  return QueryPerformanceCounter((LARGE_INTEGER*)counter);
}

////////////////////////////////////////////////////////////////////////////////
int32_t
get_keyboard_state(uint8_t key_states[256])
{
  return GetKeyboardState(key_states);
}

int16_t
get_async_key_state(int32_t key)
{
  return GetAsyncKeyState(key);
}

int32_t
show_cursor(int32_t show)
{
  return ShowCursor(show);
}

int32_t
get_cursor_position(cursor_pos_t *position)
{
  POINT point;
  BOOL result = GetCursorPos(&point);
  position->x = point.x;
  position->y = point.y;
  return result;
}

int32_t
set_cursor_position(int32_t x, int32_t y)
{
  return SetCursorPos(x, y);
}

////////////////////////////////////////////////////////////////////////////////
void
sleep(uint64_t ms)
{
  Sleep(ms);
}

////////////////////////////////////////////////////////////////////////////////
file_handle_t
find_first_file(const char *file, file_find_data_t *file_data)
{
  file_handle_t handle = INVALID_HANDLE;
  WIN32_FIND_DATA ffd;
  handle = (file_handle_t)FindFirstFile(file, &ffd);
  file_data->name = ffd.cFileName;
  file_data->file_attributes = ffd.dwFileAttributes;
  return handle;
}

int32_t
find_next_file(file_handle_t handle, file_find_data_t *file_data)
{
  WIN32_FIND_DATA ffd;
  int32_t result = FindNextFile((HANDLE)handle, &ffd);
  file_data->name = ffd.cFileName;
  file_data->file_attributes = ffd.dwFileAttributes;
  return result;
}

int32_t
find_close(file_handle_t handle)
{
  return FindClose((HWND)handle);
}

uint64_t
get_last_error()
{
  return GetLastError();
}