/**
 * @file allocator.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-01-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <library/internal/module.h>


// TODO: add stack array allocator
typedef 
struct allocator_t {
  void* (*mem_alloc)(size_t size);
  void (*mem_free)(void *ptr);
  void* (*mem_realloc)(void *ptr, size_t new_size);
  void* (*mem_cont_alloc)(size_t num, size_t size);
  void* (*mem_alloc_alligned)(size_t alignment, size_t size);
} allocator_t;

// export a default allocator
LIBRARY_API 
extern const allocator_t g_default_allocator;

#ifdef __cplusplus
}
#endif

#endif