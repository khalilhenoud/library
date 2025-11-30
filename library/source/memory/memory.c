/**
 * @file memory.c
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2025-10-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <assert.h>
#include <stdlib.h>
#include <library/memory/memory.h>


#if LIBRARY_ALLOCATION_CALLBACKS
static alloc_callback_t g_alloc_callback = NULL;
static free_callback_t g_free_callback = NULL;
static realloc_callback_t g_realloc_callback = NULL;

void
set_alloc_callback(alloc_callback_t callback)
{
  g_alloc_callback = callback;
}

void
set_free_callback(free_callback_t callback)
{
  g_free_callback = callback;
}

void
set_realloc_callback(realloc_callback_t callback)
{
  g_realloc_callback = callback;
}
#endif

void*
mem_aligned_alloc(size_t alignment, size_t size)
{
  assert(0 && "mem_aligned_alloc is not implemented!");
  return NULL;
}

void
mem_free_sized(void *ptr, size_t size)
{
  assert(0 && "mem_free_sized is not implemented!");
}

void
mem_free_aligned_sized(
  void *ptr,
  size_t alignment,
  size_t size)
{
  assert(0 && "mem_free_aligned_sized is not implemented!");
}

void*
mem_cont_alloc(size_t nmemb, size_t size)
{
#if LIBRARY_ALLOCATION_CALLBACKS
  extern alloc_callback_t g_alloc_callback;
  void *block = calloc(nmemb, size);
  if (g_alloc_callback)
    g_alloc_callback(block);
  return block;
#else
  return calloc(nmemb, size);
#endif
}

void
mem_free(void *ptr)
{
#if LIBRARY_ALLOCATION_CALLBACKS
  extern free_callback_t g_free_callback;
  if (g_free_callback)
    g_free_callback(ptr);
#endif
  free(ptr);
}

void*
mem_alloc(size_t size)
{
#if LIBRARY_ALLOCATION_CALLBACKS
  extern alloc_callback_t g_alloc_callback;
  void *block = malloc(size);
  if (g_alloc_callback)
    g_alloc_callback(block);
  return block;
#else
  return malloc(size);
#endif
}

void*
mem_realloc(
  void *ptr,
  size_t size)
{
#if LIBRARY_ALLOCATION_CALLBACKS
  extern realloc_callback_t g_realloc_callback;
  void *block = realloc(ptr, size);
  if (g_realloc_callback)
    g_realloc_callback(ptr, block);
  return block;
#else
  return realloc(ptr, size);
#endif
}
