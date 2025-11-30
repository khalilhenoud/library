/**
 * @file default_registry.c
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2025-02-15
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <library/containers/chashmap.h>
#include <library/containers/clist.h>
#include <library/containers/cvector.h>
#include <library/core/core.h>
#include <library/string/cstring.h>
#include <library/type_registry/type_registry.h>


INITIALIZER(register_cvector_t)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = cvector_def;
  vtable.fn_is_def = cvector_is_def;
  vtable.fn_replicate = cvector_replicate;
  vtable.fn_fullswap = cvector_fullswap;
  vtable.fn_serialize = cvector_serialize;
  vtable.fn_deserialize = cvector_deserialize;
  vtable.fn_hash = NULL;
  vtable.fn_is_equal = NULL;
  vtable.fn_type_size = cvector_type_size;
  vtable.fn_type_alignment = NULL;
  vtable.fn_type_id_count = cvector_type_id_count;
  vtable.fn_type_ids = cvector_type_ids;
  vtable.fn_owns_alloc = cvector_owns_alloc;
  vtable.fn_get_alloc = cvector_get_alloc;
  vtable.fn_cleanup = cvector_cleanup;
  register_type(get_type_id(cvector_t), &vtable);
}

INITIALIZER(register_clist_t)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = clist_def;
  vtable.fn_is_def = clist_is_def;
  vtable.fn_replicate = clist_replicate;
  vtable.fn_fullswap = clist_fullswap;
  vtable.fn_serialize = clist_serialize;
  vtable.fn_deserialize = clist_deserialize;
  vtable.fn_hash = NULL;
  vtable.fn_is_equal = NULL;
  vtable.fn_type_size = clist_type_size;
  vtable.fn_type_alignment = NULL;
  vtable.fn_type_id_count = clist_type_id_count;
  vtable.fn_type_ids = clist_type_ids;
  vtable.fn_owns_alloc = clist_owns_alloc;
  vtable.fn_get_alloc = clist_get_alloc;
  vtable.fn_cleanup = clist_cleanup;
  register_type(get_type_id(clist_t), &vtable);
}

INITIALIZER(register_chashmap_t)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = chashmap_def;
  vtable.fn_is_def = chashmap_is_def;
  vtable.fn_replicate = chashmap_replicate;
  vtable.fn_fullswap = chashmap_fullswap;
  vtable.fn_serialize = chashmap_serialize;
  vtable.fn_deserialize = chashmap_deserialize;
  vtable.fn_hash = NULL;
  vtable.fn_is_equal = NULL;
  vtable.fn_type_size = chashmap_type_size;
  vtable.fn_type_alignment = NULL;
  vtable.fn_type_id_count = chashmap_type_id_count;
  vtable.fn_type_ids = chashmap_type_ids;
  vtable.fn_owns_alloc = chashmap_owns_alloc;
  vtable.fn_get_alloc = chashmap_get_alloc;
  vtable.fn_cleanup = chashmap_cleanup;
  register_type(get_type_id(chashmap_t), &vtable);
}

INITIALIZER(register_cstring_t)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = cstring_def;
  vtable.fn_is_def = cstring_is_def;
  vtable.fn_replicate = cstring_replicate;
  vtable.fn_fullswap = cstring_fullswap;
  vtable.fn_serialize = cstring_serialize;
  vtable.fn_deserialize = cstring_deserialize;
  vtable.fn_hash = cstring_hash;
  vtable.fn_is_equal = cstring_is_equal;
  vtable.fn_type_size = cstring_type_size;
  vtable.fn_type_alignment = NULL;
  vtable.fn_type_id_count = NULL;
  vtable.fn_type_ids = NULL;
  vtable.fn_owns_alloc = cstring_owns_alloc;
  vtable.fn_get_alloc = cstring_get_alloc;
  vtable.fn_cleanup = cstring_cleanup;
  register_type(get_type_id(cstring_t), &vtable);
}

uint32_t
type_equal_1b(const void* key1, const void* key2)
{
  return *(uint8_t *)(key1) == *(uint8_t *)(key2);
}

size_t type_size_1b(void) { return 1; }
uint32_t
hash_1bytes(const void *_ptr)
{
  assert(_ptr);
  {
    const uint8_t *ptr = (const uint8_t *)_ptr;
    return hash_fnv1a_32(ptr, 1);
  }
}

uint32_t
type_equal_2b(const void* key1, const void* key2)
{
  return *(uint16_t *)(key1) == *(uint16_t *)(key2);
}

size_t type_size_2b(void) { return 2; }
uint32_t
hash_2bytes(const void *_ptr)
{
  assert(_ptr);
  {
    const uint16_t *ptr = (const uint16_t *)_ptr;
    return hash_fnv1a_32(ptr, 2);
  }
}

uint32_t
type_equal_4b(const void* key1, const void* key2)
{
  return *(uint32_t *)(key1) == *(uint32_t *)(key2);
}

uint32_t
type_equal_4bf(const void* key1, const void* key2)
{
  return *(float *)(key1) == *(float *)(key2);
}

size_t type_size_4b(void) { return 4; }
uint32_t
hash_4bytes(const void *_ptr)
{
  assert(_ptr);
  {
    const uint32_t *ptr = (const uint32_t *)_ptr;
    return hash_fnv1a_32(ptr, 4);
  }
}

uint32_t
type_equal_8b(const void* key1, const void* key2)
{
  return *(uint64_t *)(key1) == *(uint64_t *)(key2);
}

uint32_t
type_equal_8bf(const void* key1, const void* key2)
{
  return *(double *)(key1) == *(double *)(key2);
}

size_t type_size_8b(void) { return 8; }
uint32_t
hash_8bytes(const void *_ptr)
{
  assert(_ptr);
  {
    const uint64_t *ptr = (const uint64_t *)_ptr;
    return hash_fnv1a_32(ptr, 8);
  }
}

INITIALIZER(register_default_types_t)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_hash = hash_1bytes;
  vtable.fn_type_size = type_size_1b;
  vtable.fn_is_equal = type_equal_1b;
  register_type(get_type_id(uint8_t), &vtable);
  register_type(get_type_id(int8_t), &vtable);
  register_type(get_type_id(char), &vtable);
  register_type(get_type_id(unsigned char), &vtable);

  vtable.fn_hash = hash_2bytes;
  vtable.fn_type_size = type_size_2b;
  vtable.fn_is_equal = type_equal_2b;
  register_type(get_type_id(uint16_t), &vtable);
  register_type(get_type_id(int16_t), &vtable);
  register_type(get_type_id(short), &vtable);
  register_type(get_type_id(unsigned short), &vtable);

  vtable.fn_hash = hash_4bytes;
  vtable.fn_type_size = type_size_4b;
  vtable.fn_is_equal = type_equal_4b;
  register_type(get_type_id(uint32_t), &vtable);
  register_type(get_type_id(int32_t), &vtable);
  register_type(get_type_id(int), &vtable);
  register_type(get_type_id(unsigned int), &vtable);

  // floats can have the same value but different underlying bit patterns.
  vtable.fn_is_equal = type_equal_4bf;
  register_type(get_type_id(float), &vtable);

  vtable.fn_hash = hash_8bytes;
  vtable.fn_type_size = type_size_8b;
  vtable.fn_is_equal = type_equal_8b;
  register_type(get_type_id(uint64_t), &vtable);
  register_type(get_type_id(int64_t), &vtable);
  register_type(get_type_id(size_t), &vtable);
  register_type(get_type_id(long long), &vtable);
  register_type(get_type_id(unsigned long long), &vtable);

  vtable.fn_is_equal = type_equal_8bf;
  register_type(get_type_id(double), &vtable);
}