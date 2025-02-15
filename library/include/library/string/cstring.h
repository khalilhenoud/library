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
#include <assert.h>
#include <string.h>
#include <library/allocator/allocator.h>
#include <library/hash/fnv.h>


////////////////////////////////////////////////////////////////////////////////
//| cstring_t, '*' = cstring
//|=============================================================================
//| OPERATION                   | SUPPORTED
//|=============================================================================
//|    *_def                    | YES
//|    *_is_def                 | YES
//|    *_replicate              | YES
//|    *_fullswap               | YES
//|    *_serialize              | YES
//|    *_deserialize            | YES
//|    *_hash                   | YES
//|    *_is_equal               | YES
//|    *_type_size              | YES
//|    *_type_alignment         |
//|    *_type_id_count          |
//|    *_type_ids               |
//|    *_owns_alloc             | YES
//|    *_get_alloc              | YES
//|    *_cleanup                | YES
////////////////////////////////////////////////////////////////////////////////

typedef struct binary_stream_t binary_stream_t;

typedef
struct cstring_t {
  uint8_t *str;
  uint32_t length;
  const allocator_t *allocator;
} cstring_t;

inline
void 
cstring_def(void *ptr)
{
  assert(ptr);
  memset(ptr, 0, sizeof(cstring_t));
}

inline
uint32_t 
cstring_is_def(const void *ptr)
{
  assert(ptr);

  {
    const cstring_t *cstr = (const cstring_t *)ptr;
    return cstr->str == NULL && !cstr->length && cstr->allocator == NULL;
  }
}

/** 
 * NOTE: will assert if 'src' is not initialized, or if 'dst' is initialized but
 * with non-zero size.
 */
void 
cstring_replicate(
  const void *src, 
  void *dst, 
  const allocator_t* allocator);

void 
cstring_fullswap(void* lhs, void* rhs);

void 
cstring_serialize(
  const void *src, 
  binary_stream_t* stream);

void 
cstring_deserialize(
  void *dst, 
  const allocator_t *allocator, 
  binary_stream_t* stream);

uint32_t 
cstring_hash(const void *ptr);

uint32_t 
cstring_is_equal(
  const void *lhs, 
  const void *rhs);

inline
size_t 
cstring_type_size(void)
{
  return sizeof(cstring_t);
}

inline
uint32_t 
cstring_owns_alloc(void)
{
  return 1;
}

inline
const allocator_t* 
cstring_get_alloc(const void *ptr)
{
  const cstring_t *cstr = (const cstring_t *)ptr;
  assert(ptr);
  return cstr->allocator;
}

void 
cstring_cleanup(void *ptr, const allocator_t* allocator);

////////////////////////////////////////////////////////////////////////////////

// clears the internal string.
void 
cstring_clear(cstring_t* string);

// assign str to the string and return its size.
uint32_t 
cstring_assign(cstring_t *string, const uint8_t *str);

void
cstring_setup(
  cstring_t *string,
  const uint8_t *str, 
  const allocator_t *allocator);

#include "string.impl"

#ifdef __cplusplus
}
#endif

#endif