/**
 * @file chashtable.h
 * @author khalilhenoud@gmail.com
 * @brief hash table (linear probing)
 * @version 0.1
 * @date 2024-09-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_HASHTABLE_H
#define LIB_HASHTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>
#include <library/allocator/allocator.h>
#include <library/containers/cvector.h>


typedef cvector_elem_cleanup_t chashtable_key_cleanup_t;
typedef cvector_elem_cleanup_t chashtable_value_cleanup_t;
typedef cvector_elem_replicate_t chashtable_key_replicate_t;
typedef cvector_elem_replicate_t chashtable_value_replicate_t;

typedef
int32_t (*chashtable_key_equal_t)(const void* key1, const void* key2);

typedef
uint64_t (*chashtable_hash_calc_t)(const void* key);

// a key can be any type
// i require a 
//  - function to make a copy of the key, we shouldn't leave a reference to the 
//    original key (can be defaulted, at which point we do a byte wise copy)
//  - function to delete the key (can be default, same logic), assert if the 
//    deletion is set without the deletion.
// a value can be any type.
//  - function to delete the elem.
// we require an
//  - array that holds keys
//  - array that holds the values (this array and the keys array are of the 
//  same size)
//  - array that holds the indexes into the value's array, this is our hashmap
//  - all 3 arrays have the same size.

// NOTE: the indices is a vector of uint32_t.
typedef
struct chashtable_t {
  cvector_t keys;
  cvector_t values;
  cvector_t indices;
  float max_load_factor;
  const allocator_t* allocator;
  chashtable_key_replicate_t key_replicate;
  chashtable_key_equal_t key_equal;
  chashtable_hash_calc_t hash_calc;
} chashtable_t;

/** returns a default initialized copy of the struct */
inline
chashtable_t
chashtable_def()
{
  chashtable_t def;
  memset(&def, 0, sizeof(chashtable_t));
  return def;
}

/** returns 1 if the passed struct is the same as the default */
inline
int32_t
chashtable_is_def(const chashtable_t* hashtable)
{
  chashtable_t def = chashtable_def();
  return 
    cvector_is_def(&(hashtable->keys)) && 
    cvector_is_def(&(hashtable->values)) && 
    cvector_is_def(&(hashtable->indices)) && 
    hashtable->max_load_factor == def.max_load_factor &&
    hashtable->allocator == def.allocator && 
    hashtable->key_replicate == def.key_replicate && 
    hashtable->key_equal == def.key_equal && 
    hashtable->hash_calc == def.hash_calc;
}

/** will not allocate until the first use. */
void
chashtable_setup(
  chashtable_t* hashtable, 
  size_t key_size,
  size_t elem_size,
  const allocator_t* allocator,
  const float max_load_factor,
  chashtable_key_cleanup_t key_cleanup_fn,
  chashtable_key_replicate_t key_replicate_fn,
  chashtable_key_equal_t key_equal_fn,
  chashtable_value_cleanup_t elem_cleanup_fn,
  chashtable_hash_calc_t hash_calc_fn);

/** frees all internal memory, call setup again to reuse. */
void
chashtable_cleanup(chashtable_t* hashtable);

/** NOTE: follows the rules established by cvector in terms of assertion. */
void 
chashtable_replicate(
  const chashtable_t *src, 
  chashtable_t *dst, 
  const allocator_t *allocator, 
  cvector_elem_replicate_t elem_replicate_fn);

/** exchanges the content of the specified containers, even allocators */
void
chashtable_fullswap(chashtable_t* src, chashtable_t* dst);

size_t
chashtable_key_size(const chashtable_t* hashtable);

size_t
chashtable_value_size(const chashtable_t* hashtable);

const allocator_t*
chashtable_allocator(const chashtable_t* hashtable);

// returns the cleanup function of the key type
chashtable_key_cleanup_t
chashtable_key_cleanup(const chashtable_t* hashtable);

// returns the replicate function of the key type
chashtable_key_replicate_t
chashtable_key_replicate(const chashtable_t* hashtable);

// returns the equal function fo the key type
chashtable_key_equal_t
chashtable_key_equal(const chashtable_t* hashtable);

// returns the hashing function
chashtable_hash_calc_t
chashtable_hash_calc(const chashtable_t* hashtable);

// returns the cleanup function of the value type
chashtable_value_cleanup_t
chashtable_value_cleanup(const chashtable_t* hashtable);

// 1 if empty, 0 otherwise
int32_t
chashtable_empty(const chashtable_t* hashtable);

// returns the size of the value vector
size_t
chashtable_size(const chashtable_t* hashtable);

// returns the capacity of the value vector
size_t
chashtable_capacity(const chashtable_t* hashtable);

// clears the contents, and resizes to default capacity.
void
chashtable_clear(chashtable_t* hashtable);

// retuns the current load factor.
float
chashtable_load_factor(chashtable_t* hashtable);

// sets the max load factor, might trigger a rehash
void
chashtable_set_max_load_factor(
  chashtable_t* hashtable, 
  const float max_load_factor);

// reserves a certain number of buckets and rehashes the table
void
chashtable_rehash(chashtable_t* hashtable, size_t count);

// reserve count number of elements, assumes table is emtpy.
void
chashtable_reserve(chashtable_t* hashtable, size_t count);

#define CHASHTABLE_INVALID_INDEX ((uint32_t)-1)
#define CHASHTABLE_INIT_SIZE 16

/** the grow rate of a hashtable starts at 16 and doubles */
#define chashtable_compute_next_grow(capacity) \
  ((capacity) >= CHASHTABLE_INIT_SIZE ? (capacity * 2) : CHASHTABLE_INIT_SIZE)

#define chashtable_insert(hashtable, key, key_type, value, value_type)         \
  do {                                                                         \
    assert((hashtable) && !chashtable_is_def(hashtable));                      \
                                                                               \
    if (chashtable_load_factor(hashtable) >= (hashtable)->max_load_factor)     \
      chashtable_rehash(                                                       \
        (hashtable),                                                           \
        chashtable_compute_next_grow(chashtable_capacity(hashtable)));         \
                                                                               \
    {                                                                          \
      uint32_t index;                                                          \
      key_type scopy = (key);                                                  \
      chashtable_contains((hashtable), (key), key_type, index);           \
      if (index != CHASHTABLE_INVALID_INDEX) {                            \
        if ((hashtable)->key_replicate) { \
        cvector_cleanup_at(&(hashtable)->keys, index);       \
        (hashtable)->key_replicate( \
          &scopy, \
          cvector_at(&(hashtable)->keys, index), (hashtable)->allocator); \
        } \
        cvector_cleanup_at(&(hashtable)->values, index);                       \
        *cvector_as(&(hashtable)->values, index, value_type) = (value); \
      } else {                                                                 \
        uint64_t count = cvector_size(&(hashtable)->indices);                  \
        key_type scopy = (key);                                                \
        uint32_t hashed = (uint32_t)(hashtable)->hash_calc(&scopy) % count;    \
        if ((hashtable)->key_replicate) { \
          uint32_t last_index = cvector_size(&(hashtable)->keys); \
          cvector_resize( \
            &(hashtable)->keys, last_index + 1); \
          (hashtable)->key_replicate( \
          &scopy, \
          cvector_at(&(hashtable)->keys, last_index), (hashtable)->allocator); \
        } else \
          cvector_push_back(&(hashtable)->keys, (key), key_type);                \
        cvector_push_back(&(hashtable)->values, (value), value_type);          \
        while (                                                                \
          *cvector_as(&(hashtable)->indices, hashed, uint32_t) !=              \
          CHASHTABLE_INVALID_INDEX)                                            \
          hashed = (hashed + 1) % count;                                       \
        *cvector_as(&(hashtable)->indices, hashed, uint32_t) =                 \
          (uint32_t)cvector_size(&(hashtable)->keys) - 1;                      \
      }                                                                        \
    }                                                                          \
  } while (0)

// removes the key and value that corresponds to it.
#define chashtable_erase(hashtable, key)                                       \
  do {                                                                         \
    assert((hashtable) && !chashtable_is_def(hashtable));                      \
                                                                               \
    {                                                                          \
      uint32_t* val;                                                           \
      uint32_t data_index, i, count = cvector_size(&(hashtable)->indices);     \
      chashtable_contains((hashtable), (key), data_index);                     \
      if (data_index != CHASHTABLE_INVALID_INDEX) {                            \
        cvector_erase(&(hashtable)->keys, data_index);                         \
        cvector_erase(&(hashtable)->values, data_index);                       \
        for (i = 0; i < count; ++i) {                                          \
          val = cvector_as(&(hashtable)->indices, i, uint32_t);                \
          *val = (*val == data_index) ?                                        \
            CHASHTABLE_INVALID_INDEX :                                         \
            ((*val > data_index) ? (*val - 1) : *val);                         \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

// sets index to that of the key-value that match or to CHASHTABLE_INVALID_INDEX
#define chashtable_contains(hashtable, key, key_type, index)                   \
  do {                                                                         \
    assert((hashtable) && !chashtable_is_def(hashtable));                      \
                                                                               \
    {                                                                          \
      uint64_t count = cvector_size(&(hashtable)->indices);                    \
      key_type scopy = (key);                                                  \
      uint32_t hashed = (uint32_t)(hashtable)->hash_calc(&scopy) % count;      \
      (index) = *cvector_as(&(hashtable)->indices, hashed, uint32_t);          \
      while ((index) != CHASHTABLE_INVALID_INDEX) {                            \
        if ((hashtable)->key_equal(                                            \
          cvector_at(&(hashtable)->keys, (index)), &scopy))                    \
          break;                                                               \
        hashed = (hashed + 1) % count;                                         \
        (index) = *cvector_as(&(hashtable)->indices, hashed, uint32_t);        \
      }                                                                        \
    }                                                                          \
  } while (0)

// sets value_ptr to the address of the element or NULL
#define chashtable_at(hashtable, key, key_type, value_type, value_ptr)         \
  do {                                                                         \
    assert((hashtable) && !chashtable_is_def(hashtable));                      \
                                                                               \
    {                                                                          \
      uint32_t index;                                                          \
      chashtable_contains((hashtable), (key), key_type, index);                \
      (value_ptr) = NULL;                                                      \
      (value_ptr) = (index == CHASHTABLE_INVALID_INDEX) ? (value_ptr) :        \
        cvector_as(&(hashtable)->values, index, value_type);                   \
    }                                                                          \
  } while (0)

#include "chashtable.inl"

#ifdef __cplusplus
}
#endif

#endif