/**
 * @file cvector.inl
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-08-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_VECTOR_INL
#define LIB_VECTOR_INL


inline
void
cvector_setup(
  cvector_t* vec,
  size_t elem_size, 
  size_t capacity, 
  const allocator_t* allocator, 
  cvector_elem_cleanup_t elem_cleanup_fn)
{
  assert(vec && allocator);

  {
    vec->size = 0;
    vec->elem_size = elem_size;
    vec->capacity = capacity;
    vec->allocator = allocator;
    vec->elem_cleanup = elem_cleanup_fn;

    vec->ptr = capacity ? allocator->mem_alloc(capacity * elem_size) : NULL;
  }
}

inline
void
cvector_cleanup(cvector_t* vec)
{
  assert(vec && !cvector_is_def(vec));
  {
    if (vec->elem_cleanup) {
      size_t i = 0, count = vec->size;
      for (; i < count; ++i)
        vec->elem_cleanup(cvector_at(vec, i), vec->allocator);
    }

    vec->allocator->mem_free(vec->ptr);
    vec->ptr = NULL;
    vec->allocator = NULL;
    vec->elem_cleanup = NULL;
    vec->size = vec->elem_size = vec->capacity = 0;
  }
}

inline
void 
cvector_replicate(
  const cvector_t *src, 
  cvector_t *dst, 
  const allocator_t *allocator, 
  cvector_elem_replicate_t elem_replicate_fn)
{
  assert(src && !cvector_is_def(src));
  assert(dst);
  assert(
    (cvector_is_def(dst) && allocator) || 
    (
      !cvector_is_def(dst) && 
      !allocator && 
      dst->elem_size == src->elem_size && 
      dst->size == 0 && 
      dst->elem_cleanup == src->elem_cleanup));

  if (cvector_is_def(dst)) {
    cvector_setup(
      dst, src->elem_size, src->capacity, allocator, src->elem_cleanup);
  } else
    cvector_grow(dst, src->capacity);
  dst->size = src->size;

  if (!elem_replicate_fn)
    memcpy(dst->ptr, src->ptr, src->size * src->elem_size);
  else {
    size_t i = 0, count = src->size;
    for (; i < count; ++i)
      elem_replicate_fn(
        cvector_at_cst(src, i), cvector_at(dst, i), dst->allocator);
  }
}

inline
void
cvector_fullswap(cvector_t* src, cvector_t* dst)
{
  assert(src && dst);
  {
    cvector_t tmp = *src;
    *src = *dst;
    *dst = tmp;
  }
}

inline
size_t 
cvector_capacity(const cvector_t* vec) 
{ 
  assert(vec && !cvector_is_def(vec)); 
  return vec->capacity; 
}

inline
size_t
cvector_size(const cvector_t* vec) 
{ 
  assert(vec && !cvector_is_def(vec)); 
  return vec->size; 
}

inline
size_t
cvector_elem_size(const cvector_t* vec) 
{ 
  assert(vec && !cvector_is_def(vec));  
  return vec->elem_size; 
}

inline
const allocator_t*
cvector_allocator(const cvector_t* vec) 
{ 
  assert(vec && !cvector_is_def(vec)); 
  return vec->allocator; 
} 

inline
cvector_elem_cleanup_t
cvector_elem_cleanup(const cvector_t* vec) 
{ 
  assert(vec && !cvector_is_def(vec)); 
  return vec->elem_cleanup; 
} 

inline
int32_t
cvector_empty(const cvector_t* vec) 
{ 
  assert(vec && !cvector_is_def(vec)); 
  return vec->size == 0;
}

inline
void
cvector_grow(cvector_t* vec, size_t new_capacity)
{
  assert(vec && !cvector_is_def(vec));
  assert(
    new_capacity >= vec->size && 
    "cannot shrink beyond the current elem count!");

  {
    const size_t to_realloc = new_capacity * vec->elem_size;
    if (!to_realloc) {
      vec->allocator->mem_free(vec->ptr);
      vec->ptr = NULL;
      vec->capacity = new_capacity;
    } else {
      void* new_ptr = vec->allocator->mem_realloc(vec->ptr, to_realloc);
      assert(new_ptr);
      vec->ptr = new_ptr;
      vec->capacity = new_capacity;
    }
  }
}

inline
void
cvector_reserve(cvector_t* vec, size_t capacity)
{
  assert(vec);
  {
    if (capacity > vec->capacity)
      cvector_grow(vec, capacity);
  }
}

/** NOTE: internal use only. use this if size will be updated afterwards;. */
inline
void*
cvector_at_unchecked(cvector_t* vec, size_t index)
{
  assert(vec);
  return (char*)vec->ptr + index * vec->elem_size;
}

inline
void*
cvector_at(cvector_t* vec, size_t index)
{
  assert(vec);
  assert(index < vec->size);
  return (char*)vec->ptr + index * vec->elem_size;
}

inline
const void*
cvector_at_cst(const cvector_t* vec, size_t index)
{
  assert(vec);
  assert(index < vec->size);
  return (char*)vec->ptr + index * vec->elem_size;
}

inline
void
cvector_erase(cvector_t* vec, size_t index)
{
  assert(vec);
  assert(index < vec->size);
  {
    if (vec->elem_cleanup)
      vec->elem_cleanup(cvector_at(vec, index), vec->allocator);
    --vec->size; 
    memmove(
      cvector_at_unchecked(vec, index), 
      cvector_at_unchecked(vec, index + 1), 
      vec->elem_size * (vec->size - index));
  }
}

inline
void
cvector_clear(cvector_t* vec)
{
  assert(vec);
  {
    if (vec->elem_cleanup) {
      size_t i = 0, count = vec->size;
      for (; i < count; ++i)
        vec->elem_cleanup(cvector_at(vec, i), vec->allocator);
    }
    vec->size = 0;
  }
}

inline
void
cvector_shrink_to_fit(cvector_t* vec)
{
  assert(vec);
  cvector_grow(vec, vec->size);
}

inline
void
cvector_pop_back(cvector_t* vec)
{
  assert(vec && vec->size);
  {
    if (vec->elem_cleanup)
      vec->elem_cleanup(cvector_at(vec, vec->size - 1), vec->allocator);
    --vec->size;
  }
}

inline
void
cvector_resize(cvector_t* vec, size_t count)
{
  assert(vec && !cvector_is_def(vec));
  {
    if (count > vec->size) {
      cvector_reserve(vec, count);
      memset(
        cvector_at_unchecked(
          vec, vec->size), 0, (count - vec->size) * vec->elem_size);
      vec->size = count;
    } else {
      while (count < vec->size)
        cvector_pop_back(vec);
    }
  }
}

#endif