/**
 * @file chashmap.h
 * @author khalilhenoud@gmail.com
 * @brief hash table (linear probing)
 * @version 0.1
 * @date 2024-09-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_HASHMAP_H
#define LIB_HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <library/allocator/allocator.h>
#include <library/containers/cvector.h>
#include <library/type_registry/type_registry.h>


////////////////////////////////////////////////////////////////////////////////
//| chashmap_t, * = chashmap
//|=============================================================================
//| OPERATION                   | SUPPORTED
//|=============================================================================
//|    *_def                    | YES
//|    *_is_def                 | YES
//|    *_replicate              | YES
//|    *_fullswap               | YES
//|    *_serialize              | YES
//|    *_deserialize            | YES
//|    *_hash                   |
//|    *_is_equal               |
//|    *_type_size              | YES
//|    *_type_alignment         |
//|    *_type_id_count          | YES
//|    *_type_ids               | YES
//|    *_owns_alloc             | YES
//|    *_get_alloc              | YES
//|    *_cleanup                | YES
////////////////////////////////////////////////////////////////////////////////
// NOTES:
// - the indices is a vector of uint32_t
// - The hashmap will replicate the keys when a replicate function is
//   provided.
// - The values are inserted by copy only, there is no ownership taken(that
//   might change in the future with the implementation of a type registry).
// - If a key replicate function is provided, a key cleanup function should also
//   be provided. An assert is triggered otherwise.
// - Right now we have 3 vectors, each containing the key, value and indices
//   where the elements are kept. This will change, as I intend to either add a
//   stack to indicate gaps in the data-key vector (to improve performance) or a
//   key-value pair implementation akin to C++ unordered_map.
////////////////////////////////////////////////////////////////////////////////

typedef struct binary_stream_t binary_stream_t;

typedef
struct chashmap_t {
  cvector_t keys;
  cvector_t values;
  cvector_t indices;
  float max_load_factor;
  const allocator_t* allocator;
} chashmap_t;

typedef
struct chashmap_iterator_t {
  chashmap_t *map;
  size_t index;
} chashmap_iterator_t;

inline
void 
chashmap_def(void *ptr)
{
  assert(ptr);
  memset(ptr, 0, sizeof(chashmap_t));
}

inline
uint32_t 
chashmap_is_def(const void *ptr)
{
  chashmap_t def; 
  chashmap_def(&def);
  const chashmap_t* hashmap = (const chashmap_t *)ptr;
  return 
    cvector_is_def(&(hashmap->keys)) && 
    cvector_is_def(&(hashmap->values)) && 
    cvector_is_def(&(hashmap->indices)) && 
    hashmap->max_load_factor == def.max_load_factor &&
    hashmap->allocator == def.allocator;
}

/** NOTE: follows the rules established by cvector in terms of assertion. */
void 
chashmap_replicate(
  const void *src, 
  void *dst, 
  const allocator_t *allocator);

void
chashmap_fullswap(void* lhs, void* rhs);

void 
chashmap_serialize(
  const void *src, 
  binary_stream_t* stream);

void 
chashmap_deserialize(
  void *dst, 
  const allocator_t *allocator, 
  binary_stream_t* stream);

inline
size_t 
chashmap_type_size(void)
{
  return sizeof(chashmap_t);
}

inline
uint32_t 
chashmap_type_id_count(void)
{
  return 2;
}

inline
void 
chashmap_type_ids(const void *src, type_id_t *ids)
{
  assert(src && ids);
  {
    const chashmap_t *map = (const chashmap_t *)src;
    ids[0] = map->keys.elem_data.type_id;
    ids[1] = map->values.elem_data.type_id;
  }
}

inline
uint32_t 
chashmap_owns_alloc(void)
{
  return 1; 
}

inline
const allocator_t* 
chashmap_get_alloc(const void *ptr)
{
  assert(ptr && !chashmap_is_def(ptr));

  {
    const chashmap_t *map = (const chashmap_t *)ptr;
    return map->allocator;
  }
}

void
chashmap_cleanup(
  void *hashmap, 
  const allocator_t *allocator);

////////////////////////////////////////////////////////////////////////////////
/** will not allocate until the first use. */
void
chashmap_setup(
  chashmap_t* hashmap, 
  type_data_t key_type_data,
  type_data_t elem_type_data,
  const allocator_t* allocator,
  const float max_load_factor);

size_t
chashmap_key_size(const chashmap_t* hashmap);

size_t
chashmap_value_size(const chashmap_t* hashmap);

// returns the cleanup function of the key type
fn_cleanup_t
chashmap_key_cleanup(const chashmap_t* hashmap);

// returns the replicate function of the key type
fn_replicate_t
chashmap_key_replicate(const chashmap_t* hashmap);

// returns the equal function fo the key type
fn_is_equal_t
chashmap_key_equal(const chashmap_t* hashmap);

// returns the hashing function
fn_hash_t
chashmap_hash_calc(const chashmap_t* hashmap);

// 1 if empty, 0 otherwise
int32_t
chashmap_empty(const chashmap_t* hashmap);

// returns the size of the value vector
size_t
chashmap_size(const chashmap_t* hashmap);

// returns the capacity of the value vector
size_t
chashmap_capacity(const chashmap_t* hashmap);

// clears the contents, and resizes to default capacity.
void
chashmap_clear(chashmap_t* hashmap);

// retuns the current load factor.
float
chashmap_load_factor(chashmap_t* hashmap);

// retuns the current max load factor.
float
chashmap_max_load_factor(chashmap_t* hashmap);

// sets the max load factor, might trigger a rehash
void
chashmap_set_max_load_factor(
  chashmap_t* hashmap, 
  const float max_load_factor);

// reserves a certain number of buckets and rehashes the table
void
chashmap_rehash(chashmap_t* hashmap, size_t count);

// reserve count number of elements, assumes table is emtpy.
void
chashmap_reserve(chashmap_t* hashmap, size_t count);

/** returns an iterator that can be used to iterate over the map. */
chashmap_iterator_t
chashmap_begin(chashmap_t* map);

/** returns an end iterator. */
chashmap_iterator_t
chashmap_end(chashmap_t* map);

/** advance the iterator. */
void
chashmap_advance(chashmap_iterator_t* iter);

// TODO: should I use const pointer ? or since it is inline it does not matter.
/** 1 if equal, 0 otherwise */
int32_t
chashmap_iter_equal(chashmap_iterator_t left, chashmap_iterator_t right);

////////////////////////////////////////////////////////////////////////////////
#define chashmap_setup2(map__, key_type__, elem_type__) \
  do {                                                  \
    chashmap_def((map__));                              \
    chashmap_setup(                                     \
      (map__),                                          \
      get_type_data(key_type__),                        \
      get_type_data(elem_type__),                       \
      &g_default_allocator, 0.6f);                      \
  } while (0)

#define chashmap_cleanup2(map__)  \
  chashmap_cleanup((map__), NULL)

#define chashmap_key(iter, key_type) \
  ((key_type*)((iter)->map->keys.data) + (iter)->index)

#define chashmap_value(iter, value_type) \
  ((value_type*)((iter)->map->values.data) + (iter)->index)
  

#define CHASHTABLE_INVALID_INDEX ((uint32_t)-1)
#define CHASHTABLE_INIT_SIZE 16

/** the grow rate of a hashmap starts at 16 and doubles */
#define chashmap_compute_next_grow(capacity) \
  ((capacity) >= CHASHTABLE_INIT_SIZE ? (capacity * 2) : CHASHTABLE_INIT_SIZE)

#define chashmap_insert(hashmap, key, key_type, value, value_type)         \
  do {                                                                     \
    assert((hashmap) && !chashmap_is_def(hashmap));                        \
                                                                           \
    if (chashmap_load_factor(hashmap) >= (hashmap)->max_load_factor)       \
      chashmap_rehash(                                                     \
        (hashmap),                                                         \
        chashmap_compute_next_grow(chashmap_capacity(hashmap)));           \
                                                                           \
    {                                                                      \
      uint32_t index;                                                      \
      key_type scopy = (key);                                              \
      chashmap_contains((hashmap), (key), key_type, index);                \
      if (index != CHASHTABLE_INVALID_INDEX) {                             \
        if (chashmap_key_replicate(hashmap)) {                             \
        cvector_cleanup_at(&(hashmap)->keys, index);                       \
        chashmap_key_replicate(hashmap)(                                   \
          &scopy,                                                          \
          cvector_at(&(hashmap)->keys, index), (hashmap)->allocator);      \
        }                                                                  \
        cvector_cleanup_at(&(hashmap)->values, index);                     \
        *cvector_as(&(hashmap)->values, index, value_type) = (value);      \
      } else {                                                             \
        uint64_t count = cvector_size(&(hashmap)->indices);                \
        key_type scopy = (key);                                            \
        uint32_t hashed =                                                  \
          (uint32_t)(chashmap_hash_calc(hashmap)(&scopy) % count);         \
        if (chashmap_key_replicate(hashmap)) {                             \
          uint32_t last_index = cvector_size(&(hashmap)->keys);            \
          cvector_resize(                                                  \
            &(hashmap)->keys, last_index + 1);                             \
          chashmap_key_replicate(hashmap)(                                 \
          &scopy,                                                          \
          cvector_at(&(hashmap)->keys, last_index), (hashmap)->allocator); \
        } else                                                             \
          cvector_push_back(&(hashmap)->keys, (key), key_type);            \
        cvector_push_back(&(hashmap)->values, (value), value_type);        \
        while (                                                            \
          *cvector_as(&(hashmap)->indices, hashed, uint32_t) !=            \
          CHASHTABLE_INVALID_INDEX)                                        \
          hashed = (hashed + 1) % count;                                   \
        *cvector_as(&(hashmap)->indices, hashed, uint32_t) =               \
          (uint32_t)cvector_size(&(hashmap)->keys) - 1;                    \
      }                                                                    \
    }                                                                      \
  } while (0)

// removes the key and value that corresponds to it.
#define chashmap_erase(hashmap, key, key_type)                             \
  do {                                                                     \
    assert((hashmap) && !chashmap_is_def(hashmap));                        \
                                                                           \
    {                                                                      \
      uint32_t* __val;                                                     \
      uint32_t __index, __i, __count = cvector_size(&(hashmap)->indices);  \
      chashmap_contains((hashmap), (key), key_type, __index);              \
      if (__index != CHASHTABLE_INVALID_INDEX) {                           \
        cvector_erase(&(hashmap)->keys, __index);                          \
        cvector_erase(&(hashmap)->values, __index);                        \
        for (__i = 0; __i < __count; ++__i) {                              \
          __val = cvector_as(&(hashmap)->indices, __i, uint32_t);          \
          *__val = (*__val == __index) ?                                   \
            CHASHTABLE_INVALID_INDEX :                                     \
            ((*__val > __index && *__val != CHASHTABLE_INVALID_INDEX) ?    \
            (*__val - 1) : *__val);                                        \
        }                                                                  \
      }                                                                    \
    }                                                                      \
  } while (0)

// sets index to that of the key-value that match or to CHASHTABLE_INVALID_INDEX
#define chashmap_contains(hashmap, key, key_type, index)                   \
  do {                                                                     \
    assert((hashmap) && !chashmap_is_def(hashmap));                        \
                                                                           \
    {                                                                      \
      uint64_t count = cvector_size(&(hashmap)->indices);                  \
      key_type scopy = (key);                                              \
      uint32_t hashed =                                                    \
        (uint32_t)(chashmap_hash_calc(hashmap)(&scopy) % count);           \
      (index) = *cvector_as(&(hashmap)->indices, hashed, uint32_t);        \
      while ((index) != CHASHTABLE_INVALID_INDEX) {                        \
        if (chashmap_key_equal(hashmap)(                                   \
          cvector_at(&(hashmap)->keys, (index)), &scopy))                  \
          break;                                                           \
        hashed = (hashed + 1) % count;                                     \
        (index) = *cvector_as(&(hashmap)->indices, hashed, uint32_t);      \
      }                                                                    \
    }                                                                      \
  } while (0)

// sets value_ptr to the address of the element or NULL
#define chashmap_at(hashmap, key, key_type, value_type, value_ptr)         \
  do {                                                                     \
    assert((hashmap) && !chashmap_is_def(hashmap));                        \
                                                                           \
    {                                                                      \
      uint32_t index;                                                      \
      chashmap_contains((hashmap), (key), key_type, index);                \
      (value_ptr) = NULL;                                                  \
      (value_ptr) = (index == CHASHTABLE_INVALID_INDEX) ? (value_ptr) :    \
        cvector_as(&(hashmap)->values, index, value_type);                 \
    }                                                                      \
  } while (0)

#include "chashmap.impl"

#ifdef __cplusplus
}
#endif

#endif