/**
 * @file cvector.h
 * @author khalilhenoud@gmail.com
 * @brief generic c vector
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


// TODO: allow types to provide their (de)serialization and swap functions.
// TODO: support intial size with default values.
// TODO: support custom alignment.
// TODO: add cvector_fullswap_back() functions.
// TODO: will need accessor variants for const types (cvector_cbegin, etc...)

/**
 * NOTE: non-trivial types (types that have memory allocation as part of their
 * construction), and pointer types (array of pointers to objects), requires
 * custom deallocators (in some instances). This allows us the change to free
 * the types internal memory.
 */
typedef 
void (*cvector_elem_cleanup_t)(void *elem_ptr, const allocator_t* allocator);

/** NOTE: Used if deep copying elements is required */
typedef
void (*cvector_elem_replicate_t)(
  const void* src, void* dst, const allocator_t* allocator);

typedef 
struct cvector_t {
    size_t elem_size;
    size_t size;
    size_t capacity;
    const allocator_t* allocator;
    cvector_elem_cleanup_t elem_cleanup;
    void *ptr;
} cvector_t;

/** returns a default initialized copy of the struct */
inline
cvector_t
cvector_def()
{
  cvector_t def;
  memset(&def, 0, sizeof(cvector_t));
  return def;
}

/** returns 1 if the passed struct is the same as the default */
inline
int32_t
cvector_is_def(const cvector_t* vec)
{
  cvector_t def = cvector_def();
  return 
    vec->size == def.size && 
    vec->capacity == def.capacity && 
    vec->elem_size == def.elem_size && 
    vec->allocator == def.allocator && 
    vec->elem_cleanup == def.elem_cleanup && 
    vec->ptr == def.ptr;
}

/**
 * NOTE: this must be called first or prior to any operation that might require
 * memory allocation. if the allocator is not set, an assert is the standard
 * behavior of the container in such cases. will also assert if vec is not NULL.
 * NOTE: capacity can be zero at which point vec will be invalid, and cannot be
 * dereferenced. this is intended behavior. allocation after the fact will
 * remedy this.
 */
void
cvector_setup(
  cvector_t* vec, 
  size_t elem_size, 
  size_t capacity, 
  const allocator_t* allocator, 
  cvector_elem_cleanup_t elem_destructor_fn);

/** frees all internal memory, call setup again to reuse. */
void
cvector_cleanup(cvector_t* vec);

/** 
 * NOTE: will assert if 'src' is not initialized, or if 'dst' is initialized but
 * with non-zero size.
 * NOTE: either 'dst' or 'allocator' is NULL, not both (we assert).
 * NOTE: capacity is carried over from the src vector.
 */
void 
cvector_replicate(
  const cvector_t *src, 
  cvector_t *dst, 
  const allocator_t *allocator, 
  cvector_elem_replicate_t elem_replicate_fn);

/** exchanges the content of the specified vectors, even allocators */
void
cvector_fullswap(cvector_t* src, cvector_t* dst);

size_t 
cvector_capacity(const cvector_t* vec);

size_t
cvector_size(const cvector_t* vec);

size_t
cvector_elem_size(const cvector_t* vec);

const allocator_t*
cvector_allocator(const cvector_t* vec);

cvector_elem_cleanup_t
cvector_elem_cleanup(const cvector_t* vec);

int32_t
cvector_empty(const cvector_t* vec);

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

/** the grow rate starts at 4 then follows size * 1.5 */
#define cvector_compute_next_grow(size) \
  ((size) >= 4 ? ((size) + (size)/2) : 4 )

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
      ((type__*)(vec__)->ptr)[cvector_size(vec__)] = (value__);       \
      (vec__)->size++;                                                \
    }                                                                 \
  } while (0)

/** syntatic sugar, simply returns the pointer casted */
#define cvector_begin(vec, type) \
  ((type*)(vec)->ptr)

/** 
 * returns the address to one past the last element of the array 
 * NOTE: this is legal, dereferencing this pointer is undefined behavior
 */
#define cvector_end(vec, type) \
  ((vec)->ptr ? ((type*)((vec)->ptr) + (vec)->size) : NULL)

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
          (type*)((vec)->ptr) + (pos) + 1,                            \
          (type*)((vec)->ptr) + (pos),                                \
          (vec)->elem_size * ((vec)->size - (pos)));                  \
      }                                                               \
      ((type*)((vec)->ptr))[(pos)] = (val);                           \
      (vec)->size += 1;                                               \
    }                                                                 \
  } while (0)

#include "cvector.inl"

#ifdef __cplusplus
}
#endif

#endif