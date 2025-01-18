/**
 * @file type_registry.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-12-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef TYPE_REGISTRY_H
#define TYPE_REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <library/internal/module.h>
#include <library/hash/fnv.h>

#define get_type_id(type) hash_fnv1a_32(#type, strlen(#type))


typedef uint32_t type_id_t;

// forward declarations.
typedef struct allocator_t allocator_t;
typedef struct binary_stream_t binary_stream_t;

typedef void (*fn_def_t)(void *ptr);
typedef uint32_t (*fn_is_def_t)(const void *ptr);
typedef void (*fn_replicate_t)(
  const void *src, void *dst, const allocator_t* allocator);
typedef void (*fn_fullswap_t)(void* lhs, void* rhs);
typedef void (*fn_serialize_t)(const void *src, binary_stream_t* stream);
typedef void (*fn_deserialize_t)(
  void *dst, const allocator_t *allocator, binary_stream_t* stream);
typedef uint32_t (*fn_hash_t)(const void *ptr);
typedef uint32_t (*fn_is_equal_t)(const void *lhs, const void *rhs);
typedef size_t (*fn_size_t)(void);
typedef size_t (*fn_alignment_t)(void);
typedef uint32_t (*fn_type_id_count_t)(void);
typedef void (*fn_type_ids_t)(type_id_t *ids);
typedef uint32_t (*fn_owns_alloc_t)(void);
typedef const allocator_t* (*fn_get_alloc_t)(const void *ptr);
typedef void (*fn_cleanup_t)(void *ptr, const allocator_t* allocator);

typedef
struct vtable_t {
  fn_def_t            fn_def;
  fn_is_def_t         fn_is_def;
  fn_replicate_t      fn_replicate;
  fn_fullswap_t       fn_fullswap;
  fn_serialize_t      fn_serialize;
  fn_deserialize_t    fn_deserialize;
  fn_hash_t           fn_hash;
  fn_is_equal_t       fn_is_equal;
  fn_size_t           fn_size;
  fn_alignment_t      fn_alignment;
  fn_type_id_count_t  fn_type_id_count;
  fn_type_ids_t       fn_type_ids;
  fn_owns_alloc_t     fn_owns_alloc;
  fn_get_alloc_t      fn_get_alloc;
  fn_cleanup_t        fn_cleanup;
} vtable_t;

LIBRARY_API
void 
register_type(const type_id_t type, const vtable_t *ptr);

LIBRARY_API
uint32_t
is_type_registered(const type_id_t type);

LIBRARY_API
void
associate_alias(const type_id_t registered, const type_id_t alias);

LIBRARY_API
void 
get_vtable(const type_id_t type, vtable_t *ptr);

#ifdef __cplusplus
}
#endif

#endif