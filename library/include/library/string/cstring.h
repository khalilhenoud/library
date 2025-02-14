/**
 * @file string.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-12-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef LIB_STRING_H
#define LIB_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


// TODO: how this functionality between stack and heap is a bit of a problem. I
// need to address this sooner or later. This issue will re-occur with the
// different library containers implementation, I need a good solution for this
// system wide.

typedef struct allocator_t allocator_t;

/**
 * @brief holds a null terminated string and the allocator that was used to
 * create the instance and the buffer within it.
 * we cache the size of the string.
 * NOTE: currently there is no option to specify different allocators for the
 * instance and the internal buffer, so if you are using the type on stack use
 * clear_string() instead of free_string().
 */
typedef
struct string_t {
  char* str;
  uint32_t size;
  const allocator_t* allocator;
} string_t;


// allocates the string instance on the heap, reuses the allocator to allocate
// the internal buffer.
string_t* allocate_string(const char* str, const allocator_t* allocator);

// frees the dynamically allocated string_t instead, do not use after this.
void free_string(string_t* string);

// clears the string, return the previous size, preserves the allocator.
uint32_t clear_string(string_t* string);

// assigns str to string and returns the previous string size.
uint32_t assign_string(string_t* string, const char* str);

// copies the string data but keeps the original allocator
void copy_string(string_t* target, const string_t* source);

// returns 1 if equal 0 otherwise.
int32_t string_equal(const string_t* left, const string_t* right);

#include "string.impl"

#ifdef __cplusplus
}
#endif

#endif