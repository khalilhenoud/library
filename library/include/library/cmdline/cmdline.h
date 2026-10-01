/**
 * @file asset_ref.h
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2026-04-04
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef C_LIBRARY_CMDLINE
#define C_LIBRARY_CMDLINE

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define CMDLINE_ARGS_TOTAL_ENTRIES  64
#define CMDLINE_ARGS_TOTAL_BUFFER  2048


typedef
struct cmd_entries_t {
  char *ptr;
  uint32_t length;
} cmd_entries_t;

typedef
struct cmd_entries_list_t {
  cmd_entries_t entries[CMDLINE_ARGS_TOTAL_ENTRIES];
  uint32_t used;
} cmd_entries_list_t;

typedef
struct cmd_args_buffer_t {
  char data[CMDLINE_ARGS_TOTAL_BUFFER];
  uint32_t total;
} cmd_args_buffer_t;

typedef
struct cmd_repo_t {
  cmd_args_buffer_t buffer;
  cmd_entries_list_t list;
} cmd_repo_t;

uint32_t
parse_cmdline_args(cmd_repo_t *cmd_repo, int argc, char *argv[])
{
  size_t len = 0;
  for (uint32_t i = 0; i < argc; ++i) {
    len = strlen(argv[i]) + 1;
    assert(cmd_repo->buffer.total + len < CMDLINE_ARGS_TOTAL_BUFFER);
    memcpy(cmd_repo->buffer.data + cmd_repo->buffer.total, argv[i], len);
    cmd_repo->list.entries[cmd_repo->list.used].ptr = cmd_repo->buffer.data +
      cmd_repo->buffer.total;
    cmd_repo->list.entries[cmd_repo->list.used].length = len - 1;
    ++cmd_repo->list.used;
    assert(cmd_repo->list.used < CMDLINE_ARGS_TOTAL_ENTRIES);
    cmd_repo->buffer.total += len;
  }

  return cmd_repo->list.used;
}

#ifdef __cplusplus
}
#endif

#endif