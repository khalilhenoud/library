/**
 * @file cvector.h
 * @author khalilhenoud@gmail.com
 * @brief generic vector
 * @version 0.1
 * @date 2024-08-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_VECTOR_H
#define LIB_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <library/allocator/allocator.h>
#include <library/type_registry/type_registry.h>


////////////////////////////////////////////////////////////////////////////////
//| cvector_t, '*' = cvector
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
// TODO:
//  - support initial size with default values.
//  - support custom alignment.
//  - will require accessor variants for const types (cvector_cbegin, etc...)
////////////////////////////////////////////////////////////////////////////////

typedef struct binary_stream_t binary_stream_t;

typedef 
struct cvector_t {
  container_elem_data_t elem_data;
  size_t size;
  size_t capacity;
  const allocator_t* allocator;
  void *data;
} cvector_t;

inline
void
cvector_def(void *ptr)
{
  assert(ptr);
  memset(ptr, 0, sizeof(cvector_t));
}

inline
uint32_t 
cvector_is_def(const void *ptr)
{
  assert(ptr);

  {
    const cvector_t* vec = (const cvector_t *)ptr;
    cvector_t def;
    cvector_def(&def);
    return 
      vec->size == def.size && 
      vec->capacity == def.capacity && 
      elem_data_identical(&vec->elem_data, &def.elem_data) &&
      vec->allocator == def.allocator &&
      vec->data == def.data;
  }
}

/** 
 * NOTE: will assert if 'src' is not initialized, or if 'dst' is initialized but
 * with non-zero size.
 * NOTE: capacity is carried over from the src vector.
 */
void 
cvector_replicate(
  const void *src, 
  void *dst, 
  const allocator_t *allocator);

void
cvector_fullswap(void* src, void* dst);

void 
cvector_serialize(
  const void *src, 
  binary_stream_t* stream);

void 
cvector_deserialize(
  void *dst, 
  const allocator_t *allocator, 
  binary_stream_t* stream);

inline
size_t
cvector_type_size(void)
{
  return sizeof(cvector_t);
}

inline
uint32_t 
cvector_type_id_count(void)
{
  return 1;
}

inline
void 
cvector_type_ids(const void *src, type_id_t *ids)
{
  assert(src && ids);
  ids[0] = ((const cvector_t *)src)->elem_data.type_id;
}

inline
uint32_t 
cvector_owns_alloc(void)
{
  return 1;
}

inline
const allocator_t*
cvector_get_alloc(const void *vec)
{
  assert(vec);
  return ((const cvector_t *)vec)->allocator;
}

void 
cvector_cleanup(void *ptr, const allocator_t* allocator);

////////////////////////////////////////////////////////////////////////////////
/**
 * NOTE: capacity can be zero at which point vec will be invalid, and cannot be
 * dereferenced. this is intended behavior. allocation after the fact will
 * remedy this.
 */
void
cvector_setup(
  cvector_t *vec, 
  type_data_t type_data,
  size_t capacity, 
  const allocator_t* allocator);

size_t 
cvector_capacity(const cvector_t* vec);

size_t
cvector_size(const cvector_t* vec);

size_t
cvector_elem_size(const cvector_t* vec);

int32_t
cvector_empty(const cvector_t* vec);

/** internal: used to cleanup an element to reuse the space. */
void
cvector_cleanup_at(cvector_t* vec, size_t index);

/** expands capacity to max(capacity, vec->capacity) */
void
cvector_reserve(cvector_t* vec, size_t capacity);

/** returns a pointer to the element at index, asserts otherwise */
void*
cvector_at(cvector_t* vec, size_t index);
const void*
cvector_at_cst(const cvector_t* vec, size_t index);

/** removes the element at index from the vector. */
void
cvector_erase(cvector_t* vec, size_t index);

/** erases all of the elements in the vector, does not affect capacity */
void
cvector_clear(cvector_t* vec);

/** 
 * reserves 'count' * 'elem_size' in terms of capacity, internal use. 
 * NOTE: this can be used to reduce overall capacity as in shrink_to_fit()
 */
void
cvector_grow(cvector_t* vec, size_t new_capacity);

#define CVECTOR_INIT_SIZE 4

/** the grow rate starts at 4 then follows size * 1.5 */
#define cvector_compute_next_grow(size) \
  ((size) >= CVECTOR_INIT_SIZE ? ((size) + (size)/2) : CVECTOR_INIT_SIZE )

/** discards extra memory capacity. */
void
cvector_shrink_to_fit(cvector_t* vec);

/** removes the last element from the vector, memory capacity is preserved */
void
cvector_pop_back(cvector_t* vec);

/** 
 * resizes the container to contain 'count' elements, the elements are 0
 * initialized.
 * TODO: provide a variant that takes an initializer function
 */
void
cvector_resize(cvector_t* vec, size_t count);

////////////////////////////////////////////////////////////////////////////////
#define cvector_iterator(type) type *

/** adds an element to the end of the vector */
#define cvector_push_back(vec__, value__, type__)                     \
  do {                                                                \
    assert(vec__);                                                    \
    {                                                                 \
      size_t cv_cap__ = cvector_capacity(vec__);                      \
      if (cv_cap__ <= cvector_size(vec__))                            \
        cvector_grow((vec__), cvector_compute_next_grow(cv_cap__));   \
      ((type__*)(vec__)->data)[cvector_size(vec__)] = (value__);      \
      (vec__)->size++;                                                \
    }                                                                 \
  } while (0)

/** syntatic sugar, simply returns the pointer casted */
#define cvector_begin(vec, type) \
  ((type*)(vec)->data)

/** 
 * returns the address to one past the last element of the array 
 * NOTE: this is legal, dereferencing this pointer is undefined behavior
 */
#define cvector_end(vec, type) \
  ((vec)->data ? ((type*)((vec)->data) + (vec)->size) : NULL)

/** returns a casted pointer to the element at n position or NULL if invalid */
#define cvector_as(vec, n, type) \
  (type*)cvector_at((vec), (n))

/** returns a pointer to the first element or NULL*/
#define cvector_front(vec, type) \
  cvector_as(vec, 0, type)

/** returns a pointer to the last element or NULL */
#define cvector_back(vec, type) \
  cvector_as(vec, (vec)->size - 1, type)

/** insert element 'val' at 'pos' in the vector */
#define cvector_insert(vec, pos, val, type)                           \
  do {                                                                \
    assert((vec) && !cvector_is_def(vec));                            \
    assert((pos) >= 0 && (pos) <= cvector_size(vec));                 \
    {                                                                 \
      size_t cv_cap__ = cvector_capacity(vec);                        \
      if (cv_cap__ <= cvector_size(vec))                              \
        cvector_grow((vec), cvector_compute_next_grow(cv_cap__));     \
      if ((pos) < cvector_size(vec)) {                                \
        memmove(                                                      \
          (type*)((vec)->data) + (pos) + 1,                           \
          (type*)((vec)->data) + (pos),                               \
          (vec)->elem_data.size * ((vec)->size - (pos)));             \
      }                                                               \
      ((type*)((vec)->data))[(pos)] = (val);                          \
      (vec)->size += 1;                                               \
    }                                                                 \
  } while (0)

#include "cvector.impl"

#ifdef __cplusplus
}
#endif

#endif