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

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <library/internal/module.h>
#include <library/hash/fnv.h>

#define get_type_id(type) hash_fnv1a_32(#type, strlen(#type))
#define get_type_data(type) \
  ((uint64_t)hash_fnv1a_32(#type, strlen(#type)) | (sizeof(type) << 32))
#define pack_type_data(type_hash, type_size) \
  ((uint64_t)(type_hash) | ((type_size) << 32))
#define get_type_id_from_data(type_data) ((type_data) & ((1ull << 32) - 1))
#define get_type_size_from_data(type_data) ((type_data) >> 32)


// NOTE: type_data_t high bits have the sizeof the type, low bits have the hash
typedef size_t type_data_t;
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
typedef size_t (*fn_type_size_t)(void);
typedef size_t (*fn_type_alignment_t)(void);
typedef uint32_t (*fn_type_id_count_t)(void);
typedef void (*fn_type_ids_t)(const void *src, type_id_t *ids);
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
  fn_type_size_t      fn_type_size;
  fn_type_alignment_t fn_type_alignment;
  fn_type_id_count_t  fn_type_id_count;
  fn_type_ids_t       fn_type_ids;
  fn_owns_alloc_t     fn_owns_alloc;
  fn_get_alloc_t      fn_get_alloc;
  fn_cleanup_t        fn_cleanup;
} vtable_t;

// NOTE: some types are not registered, meaning no vtable can be queried. to
// work with containers, we require at least the type size.
// size is useful for quick access and caching (one less function call).
typedef
struct container_elem_data_t {
  type_id_t type_id;
  size_t size;
  vtable_t *vtable;
} container_elem_data_t;

inline
type_data_t
get_type_data_from_elem_data(const container_elem_data_t *src)
{
  return pack_type_data(src->type_id, src->size);
}

inline
uint32_t
elem_data_identical(
  const container_elem_data_t *left,
  const container_elem_data_t *right)
{
  assert(left && right && "cannot be null!");

  return
    left->size == right->size &&
    left->type_id == right->type_id &&
    left->vtable == right->vtable;
}

inline
fn_cleanup_t
elem_data_get_cleanup_fn(const container_elem_data_t *elem)
{
  assert(elem);
  return
    (elem->vtable == NULL || elem->vtable->fn_cleanup == NULL) ?
    NULL : elem->vtable->fn_cleanup;
}

inline
fn_replicate_t
elem_data_get_replicate_fn(const container_elem_data_t *elem)
{
  assert(elem);
  return
    (elem->vtable == NULL || elem->vtable->fn_replicate == NULL) ?
    NULL : elem->vtable->fn_replicate;
}

inline
fn_hash_t
elem_data_get_hash_fn(const container_elem_data_t *elem)
{
  assert(elem);
  return
    (elem->vtable == NULL || elem->vtable->fn_hash == NULL) ?
    NULL : elem->vtable->fn_hash;
}

inline
fn_is_equal_t
elem_data_get_is_equal_fn(const container_elem_data_t *elem)
{
  assert(elem);
    return
    (elem->vtable == NULL || elem->vtable->fn_is_equal == NULL) ?
    NULL : elem->vtable->fn_is_equal;
}

inline
fn_serialize_t
elem_data_get_serialize_fn(const container_elem_data_t *elem)
{
  assert(elem);
    return
    (elem->vtable == NULL || elem->vtable->fn_serialize == NULL) ?
    NULL : elem->vtable->fn_serialize;
}

inline
fn_deserialize_t
elem_data_get_deserialize_fn(const container_elem_data_t *elem)
{
  assert(elem);
    return
    (elem->vtable == NULL || elem->vtable->fn_deserialize == NULL) ?
    NULL : elem->vtable->fn_deserialize;
}

inline
void
elem_data_clear(container_elem_data_t *elem)
{
  assert(elem);
  elem->size = 0;
  elem->type_id = 0;
  elem->vtable = NULL;
}

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
vtable_t *
get_vtable(const type_id_t type);

inline
container_elem_data_t
get_cont_elem_data(type_id_t id, size_t size)
{
  container_elem_data_t data;
  data.type_id = id;
  data.size = size;
  data.vtable = NULL;

  assert(id != 0);

  if (is_type_registered(id)) {
    data.vtable = get_vtable(id);

    if (data.vtable->fn_type_size)
      assert(data.size == data.vtable->fn_type_size() && "sizes must match!");
  }

  return data;
}

inline
container_elem_data_t
get_cont_elem_data_from_packed(type_data_t type_data)
{
  return get_cont_elem_data(
    get_type_id_from_data(type_data),
    get_type_size_from_data(type_data));
}

#ifdef __cplusplus
}
#endif

#endif