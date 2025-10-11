/**
 * @file memory.h
 * @author your name (you@domain.com)
 * @brief 
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


// TODO: implement
void* 
mem_aligned_alloc(size_t alignment, size_t size);

void* 
mem_cont_alloc(size_t nmemb, size_t size);

void 
mem_free(void *ptr);

// TODO: implement
void 
mem_free_sized(void *ptr, size_t size);

// TODO: implement
void 
mem_free_aligned_sized(
  void *ptr, 
  size_t alignment, 
  size_t size);

void* 
mem_alloc(size_t size);

void* 
mem_realloc(
  void *ptr, 
  size_t size);

#include "memory.impl"

#ifdef __cplusplus
}
#endif

#endif