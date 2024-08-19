/**
 * @file cvector.h
 * @author
 * @brief generic c vector based on "https://github.com/eteran/c-vector.git" 
 * @version 0.1
 * @date 2024-08-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_VECTOR_H
#define LIB_VECTOR_H

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <library/allocator/allocator.h>


// TODO: allow types to provide their (de)serialization and swap functions.
// TODO: support intial size with default values.
// TODO: support custom alignment.
// Q: can I unify cvector() and cvector_init(). if so, how (without void*)?

/** IMPORTANT: based on the work at https://github.com/eteran/c-vector.git */

/**
 * NOTE: we require the initial instace to be set to NULL. that is the only 
 * thing we consider in regards to whether the vector has been initialized or 
 * not (cvector_init() called or not).
 */

/**
 * NOTE: non-trivial types (types that have memory allocation as part of their
 * construction), and pointer types (array of pointers to objects), requires
 * custom deallocators (in some instances). This allows us the change to free
 * the types internal memory.
 */
typedef 
void (*cvector_elem_destructor_t)(void *elem_ptr, const allocator_t* allocator);

typedef 
struct cvector_metadata_t {
    size_t size;
    size_t capacity;
    const allocator_t* allocator;
    cvector_elem_destructor_t elem_destructor;
} cvector_metadata_t;

#define cvector_vector_type(type) type *

#define cvector(type) cvector_vector_type(type)

#define cvector_iterator(type) cvector_vector_type(type)

/** vector address to its corresponding metadata address. internal only */
#define cvector_vec_to_base(vec) \
  (&((cvector_metadata_t *)(vec))[-1])

/** metadata to vector start address conversion. internal only */
#define cvector_base_to_vec(ptr) \
  ((void *)&((cvector_metadata_t *)(ptr))[1])

#define cvector_capacity(vec) \
  ((vec) ? cvector_vec_to_base(vec)->capacity : (size_t)0)

#define cvector_size(vec) \
  ((vec) ? cvector_vec_to_base(vec)->size : (size_t)0)

#define cvector_allocator(vec) \ 
  ((vec) ? cvector_vec_to_base(vec)->allocator : NULL)

/** returns the custom destructor function associated with the vector */
#define cvector_elem_destructor(vec) \
  ((vec) ? cvector_vec_to_base(vec)->elem_destructor : NULL)

#define cvector_empty(vec) \
  (cvector_size(vec) == 0)

/**
 * NOTE: this must be called first or prior to any operation that might require
 * memory allocation. if the allocator is not set, an assert is the standard
 * behavior of the container in such cases. will also assert if vec is not NULL.
 * NOTE: capacity can be zero at which point vec will be invalid, and cannot be
 * dereferenced. this is intended behavior. allocation after the fact will
 * remedy this.
 */
#define cvector_init(vec, capacity, allocator, elem_destructor_fn)        \
  do {                                                                    \
    assert(!(vec) && "must be initialized to NULL!");                     \
    assert((allocator) && "invalid allocator");                           \
    {                                                                     \
      const size_t cv_sz__ =                                              \
        (capacity) * sizeof(*(vec)) + sizeof(cvector_metadata_t);         \
      void *cv_p__ = (allocator)->mem_alloc(cv_sz__);                     \
      assert(cv_p__);                                                     \
      (vec) = cvector_base_to_vec(cv_p__);                                \
      cvector_vec_to_base(vec)->size = 0;                                 \
      cvector_vec_to_base(vec)->capacity = (capacity);                    \
      cvector_vec_to_base(vec)->allocator = (allocator);                  \
      cvector_vec_to_base(vec)->elem_destructor = (elem_destructor_fn);   \
    }                                                                     \
  } while (0)

/** expands capacity to max(n, current_capacity) */
#define cvector_reserve(vec, n)                                 \
  do {                                                          \
    assert(vec);                                                \
    {                                                           \
      size_t cv_cap__ = cvector_capacity(vec);                  \
      if (cv_cap__ < (n))                                       \
        cvector_grow((vec), (n));                               \
    }                                                           \
  } while (0)

/** removes the element at index i from the vector. */
#define cvector_erase(vec, i)                                                  \
  do {                                                                         \
    assert(vec);                                                               \
    {                                                                          \
      const size cv_sz__ = cvector_size(vec);                                  \
      assert((i) < cv_sz__);                                                   \
      {                                                                        \
        const allocator_t* allocator__ = cvector_allocator(vec);               \
        cvector_elem_destructor_t elem_destructor__ =                          \
          cvector_elem_destructor(vec);                                        \
        if (elem_destructor__)                                                 \
          elem_destructor__(&(vec)[i], allocator__);                           \
        cvector_set_size((vec), cv_sz__ - 1);                                  \
        memmove(                                                               \
          (vec) + (i), (vec) + (i) + 1, sizeof(*(vec)) * (cv_sz__ - 1 - (i))); \
      }                                                                        \
    }                                                                          \
  } while (0)

/** erases all of the elements in the vector, does not affect capacity */
#define cvector_clear(vec)                                        \
  do {                                                            \
    assert(vec);                                                  \
    {                                                             \
      const allocator_t* allocator__ = cvector_allocator(vec);    \
      cvector_elem_destructor_t elem_destructor__ =               \
        cvector_elem_destructor(vec);                             \
      if (elem_destructor__) {                                    \
        size_t i__ = 0, count__ = cvector_size(vec);              \
        for (; i__ < count__; ++i__)                              \
          elem_destructor__(&(vec)[i__], allocator__);            \
      }                                                           \
      cvector_set_size((vec), 0);                                 \
    }                                                             \
  } while (0)

/** frees all memory associated with the vector, do not use afterwards */
#define cvector_free(vec)                                           \
  do {                                                              \
    assert(vec);                                                    \
    {                                                               \
      void *p1__ = cvector_vec_to_base(vec);                        \
      const allocator_t* allocator__ = cvector_allocator(vec);      \
      cvector_elem_destructor_t elem_destructor__ =                 \
        cvector_elem_destructor(vec);                               \
      if (elem_destructor__) {                                      \
        size_t i__ = 0, count__ = cvector_size(vec);                \
        for (; i__ < count__; ++i__)                                \
          elem_destructor__(&(vec)[i__], allocator__);              \
      }                                                             \
      allocator__->mem_free(p1__);                                  \
    }                                                               \
  } while (0)

/** syntatic sugar, simply returns the pointer */
#define cvector_begin(vec) \
  (vec)

/** 
 * returns the address to one past the last element of the array 
 * NOTE: this is legal, dereferencing this pointer is undefined behavior
 */
#define cvector_end(vec) \
  ((vec) ? ((vec) + cvector_size(vec)) : NULL)

/** the grow rate starts at 4 then follows size * 1.5 */
#define cvector_compute_next_grow(size) \
  ((size) < 4 ? ((size) + (size)/2) : 4 )

/** adds an element to the end of the vector */
#define cvector_push_back(vec, value)                                 \
  do {                                                                \
    assert(vec);                                                      \
    {                                                                 \
      size_t cv_cap__ = cvector_capacity(vec);                        \
      if (cv_cap__ <= cvector_size(vec))                              \
        cvector_grow((vec), cvector_compute_next_grow(cv_cap__));     \
      (vec)[cvector_size(vec)] = (value);                             \
      cvector_set_size((vec), cvector_size(vec) + 1);                 \
    }                                                                 \
  } while (0)

/** internal use - sets the size */
#define cvector_set_size(vec, _size)            \
  do {                                          \
    assert(vec);                                \
    cvector_vec_to_base(vec)->size = (_size);   \
  } while (0)

/** internal use - sets the capacity */
#define cvector_set_capacity(vec, _capacity)            \
  do {                                                  \
    assert(vec);                                        \
    cvector_vec_to_base(vec)->capacity = (_capacity);   \
  } while (0)

/** internal use - sets the destructor function */
#define cvector_set_elem_destructor(vec, elem_destructor_fn)          \
  do {                                                                \
    assert(vec);                                                      \
    cvector_vec_to_base(vec)->elem_destructor = (elem_destructor_fn); \
  } while (0)

/** discards extra memory capacity. */
#define cvector_shrink_to_fit(vec)                   \
  do {                                               \
    assert(vec);                                     \
    {                                                \
      const size_t cv_sz___ = cvector_size(vec);     \
      cvector_grow((vec), cv_sz___);                 \
    }                                                \
  } while (0)

/** returns a pointer to the element at n position or NULL if invalid */
#define cvector_at(vec, n)                                        \
  (                                                               \
    (vec) ?                                                       \ 
    (                                                             \
      ((int32_t)(n) < 0 || (size_t)(n) >= cvector_size(vec)) ?    \  
      NULL :                                                      \
      &(vec)[(n)]) :                                              \ 
    NULL)

/** returns a pointer to the first element or NULL, as if: cvector_at(vec, 0) */
#define cvector_front(vec) \
  ((vec) ? ((cvector_size(vec) > 0) ? cvector_at((vec), 0) : NULL) : NULL)

/** returns a pointer to the last element or NULL */
#define cvector_back(vec)                           \
  (                                                 \
    (vec) ?                                         \
    (                                               \
      (cvector_size(vec) > 0) ?                     \
      cvector_at(vec, cvector_size(vec) - 1) :      \
      NULL) :                                       \
    NULL)

/** removes the last element from the vector, memory capacity is preserved */
#define cvector_pop_back(vec)                                           \
  do {                                                                  \
    assert(vec);                                                        \
    {                                                                   \
      cvector_elem_destructor_t elem_destructor__ =                     \
        cvector_elem_destructor(vec);                                   \
      const allocator_t* allocator = cvector_allocator(vec);            \
      if (elem_destructor__)                                            \
        elem_destructor__(&(vec)[cvector_size(vec) - 1], allocator);    \
      cvector_set_size((vec), cvector_size(vec) - 1);                   \
    }                                                                   \
  } while (0)

/** NOTE: Used if deep copying elements is required */
typedef
void (*cvector_elem_copy_t)(void* src, void* dst, const allocator_t* allocator);

/** 
 * NOTE: will assert if from is not initialized, or if to is initialized but
 * with non-zero size.
 * NOTE: either to or allocator is NULL, otherwise the function will assert.
 */
#define cvector_copy(from, to, allocator, elem_copy_fn)                     \
  do {                                                                      \
    assert(from);                                                           \
    assert(                                                                 \
      (!(to) && (allocator)) ||                                             \
      ((to) && (cvector_size(to) == 0) && !(allocator)));                   \
    if (!(to)) {                                                            \
      cvector_init(                                                         \
        (to),                                                               \
        cvector_size(from),                                                 \
        allocator,                                                          \
        cvector_elem_destructor(from));                                     \
    } else                                                                  \
      cvector_grow((to), cvector_size(from));                               \
    cvector_set_size(to, cvector_size(from));                               \
    if (!elem_copy_fn)                                                      \
      memcpy((to), (from), cvector_size(from) * sizeof(*(from)));           \
    else {                                                                  \
      size_t i__ = 0, count__ = cvector_size(from);                         \
      for (i__ < count__; ++i__)                                            \
        elem_copy_fn(&(from)[i__], &(to)[i__], cvector_allocator(to));      \
    }                                                                       \
  } while (0)

/** 
 * exchanges the content of the specified vectors, must be same type 
 * NOTE: even allocators are swapped
 */
#define cvector_swap(vec, other, type)              \
  do {                                              \
    assert((vec) && (other));                       \
    {                                               \
      cvector_vector_type(type) cv_swap__ = (vec);  \
      (vec) = (other);                              \
      (other) = cv_swap__;                          \
    }                                               \
  } while (0)

/** insert element 'val' at 'pos' in the vector */
#define cvector_insert(vec, pos, val)                                 \
  do {                                                                \
    assert(vec);                                                      \
    assert((pos) >= 0 && (pos) <= cvector_size(vec));                 \
    {                                                                 \
      size_t cv_cap__ = cvector_capacity(vec);                        \
      if (cv_cap__ <= cvector_size(vec))                              \
        cvector_grow((vec), cvector_compute_next_grow(cv_cap__));     \
      if ((pos) < cvector_size(vec)) {                                \
        memmove(                                                      \
          (vec) + (pos) + 1,                                          \
          (vec) + (pos),                                              \
          sizeof(*(vec)) * ((cvector_size(vec)) - (pos)));            \
      }                                                               \
      (vec)[(pos)] = (val);                                           \
      cvector_set_size((vec), cvector_size(vec) + 1);                 \
    }                                                                 \
  } while (0)

/** 
 * reserves 'count' * 'elem_size' in terms of capacity, internal use. 
 * NOTE: this can be used to reduce overall capacity as in shrink_to_fit()
 */
#define cvector_grow(vec, count)                                      \
  do {                                                                \
    assert(vec);                                                      \
    {                                                                 \
      const size_t cv_sz__ =                                          \
        (count) * sizeof(*(vec)) + sizeof(cvector_metadata_t);        \
      const allocator_t* allocator = cvector_allocator(vec);          \
      void *cv_p1__ = cvector_vec_to_base(vec);                       \
      void *cv_p2__ = allocator->mem_realloc(cv_p1__, cv_sz__);       \
      assert(cv_p2__);                                                \
      (vec) = cvector_base_to_vec(cv_p2__);                           \
      cvector_set_capacity((vec), (count));                           \
    }                                                                 \
  } while (0)

/** 
 * resizes the container to contain 'count' elements 
 * TODO: provide a variant that takes an initializer function
 */
#define cvector_resize(vec, count, value)                   \
  do {                                                      \
    assert(vec);                                            \
    {                                                       \
      size_t cv_sz_count__ = (size_t)(count);               \
      size_t cv_sz__ = cvector_vec_to_base(vec)->size;      \
      if (cv_sz_count > cv_sz__) {                          \
        cvector_reserve((vec), cv_sz_count__);              \
        cvector_set_size((vec), cv_sz_count__);             \
        do {                                                \
          (vec)[cv_sz__++] = (value);                       \
        } while (cv_sz__ < cv_sz_count__);                  \
      } else {                                              \
        while (cv_sz_count__ < cv_sz__--)                   \
          cvector_pop_back(vec);                            \
      }                                                     \
    }                                                       \
  } while (0)

#define cvector_for_each_in(it, vec) \
  for (it = cvector_begin(vec); it < cvector_end(vec); it++)

#endif