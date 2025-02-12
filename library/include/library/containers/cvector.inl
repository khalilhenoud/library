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

#include <library/streams/binary_stream.h>


inline
void 
cvector_replicate(
  const void *v_src, 
  void *v_dst, 
  const allocator_t *allocator)
{
  const cvector_t *src = (const cvector_t *)v_src;
  cvector_t *dst = (cvector_t *)v_dst;

  assert(src && !cvector_is_def(src));
  assert(dst);
  assert(
    (cvector_is_def(dst) && allocator) || 
    (
      !cvector_is_def(dst) && 
      !allocator && 
      cvector_empty(dst) &&
      elem_data_identical(&dst->elem_data, &src->elem_data)));

  if (cvector_is_def(dst)) {
    cvector_setup(
      dst, 
      pack_type_data(src->elem_data.type_id, src->elem_data.size), 
      src->capacity, allocator);
  } else
    cvector_grow(dst, src->capacity);
  dst->size = src->size;

  if (!elem_data_get_replicate_fn(&src->elem_data))
    memcpy(dst->data, src->data, src->size * src->elem_data.size);
  else {
    fn_replicate_t replicate = elem_data_get_replicate_fn(&src->elem_data);
    size_t i = 0, count = src->size;
    for (; i < count; ++i)
      replicate(
        cvector_at_cst(src, i), cvector_at(dst, i), dst->allocator);
  }
}

inline
void
cvector_fullswap(void* src, void* dst)
{
  assert(src && dst);
  {
    cvector_t tmp = *(cvector_t *)src;
    *(cvector_t *)src = *(cvector_t *)dst;
    *(cvector_t *)dst = tmp;
  }
}

inline
void
cvector_serialize(
  const void *p_src,
  binary_stream_t* stream)
{
  const cvector_t *src = (const cvector_t *)p_src;
  assert(src && stream);

  {
    fn_serialize_t serialize = elem_data_get_serialize_fn(&src->elem_data);
    type_data_t type_data = get_type_data_from_elem_data(&src->elem_data);
    binary_stream_write(stream, &type_data, sizeof(size_t));
    binary_stream_write(stream, &src->size, sizeof(size_t));
    binary_stream_write(stream, &src->capacity, sizeof(size_t));

    if (serialize) {
      const uint8_t *data = (const uint8_t *)src->data; 
      size_t i = 0;
      for (; i < src->size; ++i)
        serialize(data + i * src->elem_data.size, stream);
    } else
      binary_stream_write(stream, src->data, src->size * src->elem_data.size);
  }
}

inline
void 
cvector_deserialize(
  void *p_dst, 
  const allocator_t *allocator, 
  binary_stream_t *stream)
{
  cvector_t *dst = (cvector_t *)p_dst;
  assert(dst && cvector_is_def(dst) && allocator && stream);

  {
    fn_deserialize_t deserialize = NULL;
    const size_t s_s = sizeof(size_t);
    type_data_t type_data;
    binary_stream_read(stream, (uint8_t *)&type_data, s_s, s_s);
    dst->elem_data = get_cont_elem_data_from_packed(type_data);
    dst->allocator = allocator;

    binary_stream_read(stream, (uint8_t *)&dst->size, s_s, s_s);
    binary_stream_read(stream, (uint8_t *)&dst->capacity, s_s, s_s);

    deserialize = elem_data_get_deserialize_fn(&dst->elem_data);
    if (dst->capacity) {
      dst->data = allocator->mem_alloc(dst->capacity * dst->elem_data.size);

      if (deserialize) {
        uint8_t *data = (uint8_t *)dst->data; 
        size_t i = 0;
        for (; i < dst->size; ++i)
          deserialize(data + i * dst->elem_data.size, allocator, stream);
      } else
        binary_stream_read(
          stream, 
          (uint8_t *)dst->data, 
          dst->size * dst->elem_data.size, dst->size * dst->elem_data.size);
    }
  }
}

inline
void
cvector_cleanup(void *ptr, const allocator_t* allocator)
{
  assert(ptr && !cvector_is_def(ptr));
  assert(!allocator && "type holds its own allocator! pass NULL as param.");

  {
    cvector_t* vec = (cvector_t *)ptr;
    fn_cleanup_t cleanup = elem_data_get_cleanup_fn(&vec->elem_data);

    if (cleanup) {
      size_t i = 0, count = vec->size;
      uint32_t owns_alloc = 
        (vec->elem_data.vtable->fn_owns_alloc == NULL) ? 0 : 
        vec->elem_data.vtable->fn_owns_alloc();
        
      for (; i < count; ++i)
        cleanup(
          cvector_at(vec, i), owns_alloc ? NULL : vec->allocator);
    }

    vec->allocator->mem_free(vec->data);
    vec->data = NULL;
    vec->allocator = NULL;
    elem_data_clear(&vec->elem_data);
    vec->size = vec->capacity = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
inline
void
cvector_setup(
  cvector_t* vec,
  type_data_t type_data,
  size_t capacity, 
  const allocator_t* allocator)
{
  assert(vec && allocator);

  {
    vec->size = 0;
    vec->elem_data = get_cont_elem_data_from_packed(type_data);
    vec->capacity = capacity;
    vec->allocator = allocator;

    vec->data = 
      capacity ? allocator->mem_alloc(capacity * vec->elem_data.size) : NULL;
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
  return vec->elem_data.size; 
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
cvector_cleanup_at(cvector_t* vec, size_t index)
{
  assert(vec && !cvector_is_def(vec));

  {
    fn_cleanup_t cleanup = elem_data_get_cleanup_fn(&vec->elem_data);
    if (cleanup)
      cleanup(cvector_at(vec, index), vec->allocator);
  }
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
    const size_t to_realloc = new_capacity * vec->elem_data.size;
    if (!to_realloc) {
      vec->allocator->mem_free(vec->data);
      vec->data = NULL;
      vec->capacity = new_capacity;
    } else {
      void* new_ptr = vec->allocator->mem_realloc(vec->data, to_realloc);
      assert(new_ptr);
      vec->data = new_ptr;
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
  return (char*)vec->data + index * vec->elem_data.size;
}

inline
void*
cvector_at(cvector_t* vec, size_t index)
{
  assert(vec);
  assert(index < vec->size);
  return (char*)vec->data + index * vec->elem_data.size;
}

inline
const void*
cvector_at_cst(const cvector_t* vec, size_t index)
{
  assert(vec);
  assert(index < vec->size);
  return (char*)vec->data + index * vec->elem_data.size;
}

inline
void
cvector_erase(cvector_t* vec, size_t index)
{
  assert(vec);
  assert(index < vec->size);
  {
    fn_cleanup_t cleanup = elem_data_get_cleanup_fn(&vec->elem_data);
    if (cleanup)
      cleanup(cvector_at(vec, index), vec->allocator);
    --vec->size; 
    memmove(
      cvector_at_unchecked(vec, index), 
      cvector_at_unchecked(vec, index + 1), 
      vec->elem_data.size * (vec->size - index));
  }
}

inline
void
cvector_clear(cvector_t* vec)
{
  assert(vec);
  {
    fn_cleanup_t cleanup = elem_data_get_cleanup_fn(&vec->elem_data);
    if (cleanup) {
      size_t i = 0, count = vec->size;
      for (; i < count; ++i)
        cleanup(cvector_at(vec, i), vec->allocator);
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
    fn_cleanup_t cleanup = elem_data_get_cleanup_fn(&vec->elem_data);
    if (cleanup)
      cleanup(cvector_at(vec, vec->size - 1), vec->allocator);
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
          vec, vec->size), 0, (count - vec->size) * vec->elem_data.size);
      vec->size = count;
    } else {
      while (count < vec->size)
        cvector_pop_back(vec);
    }
  }
}

#endif