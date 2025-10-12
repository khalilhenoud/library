/**
 * @file memory.h
 * @author khalilhenoud@gmail.com
 * @brief c style memory allocation with callbacks for tracking
 * @version 0.1
 * @date 2025-10-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef C_LIBRARY_MEMORY_H
#define C_LIBRARY_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <library/internal/module.h>
 
#ifndef LIBRARY_ALLOCATION_CALLBACKS
#define LIBRARY_ALLOCATION_CALLBACKS 1
#endif


#if LIBRARY_ALLOCATION_CALLBACKS
typedef void(*alloc_callback_t)(void*);
typedef void(*free_callback_t)(void*);
typedef void(*realloc_callback_t)(void*, void*);

LIBRARY_API
void
set_alloc_callback(alloc_callback_t callback);

LIBRARY_API
void
set_free_callback(free_callback_t callback);

LIBRARY_API
void
set_realloc_callback(realloc_callback_t callback);
#endif

// TODO: implement
LIBRARY_API
void* 
mem_aligned_alloc(size_t alignment, size_t size);

// TODO: implement
LIBRARY_API
void 
mem_free_sized(void *ptr, size_t size);

// TODO: implement
LIBRARY_API
void 
mem_free_aligned_sized(
  void *ptr, 
  size_t alignment, 
  size_t size);

LIBRARY_API
void* 
mem_cont_alloc(
  size_t nmemb, 
  size_t size);

LIBRARY_API
void 
mem_free(void *ptr);

LIBRARY_API
void* 
mem_alloc(size_t size);

LIBRARY_API
void* 
mem_realloc(
  void *ptr, 
  size_t size);

#ifdef __cplusplus
}
#endif

#endif