/**
 * @file allocator.c
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2025-10-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <library/allocator/allocator.h>
#include <library/memory/memory.h>


LIBRARY_API
const allocator_t g_default_allocator = {
  mem_alloc,
  mem_free,
  mem_realloc,
  mem_cont_alloc,
  mem_aligned_alloc };