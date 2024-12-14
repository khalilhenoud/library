/**
 * @file chashtable.inl
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-09-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_HASHTABLE_INL
#define LIB_HASHTABLE_INL

#include <math.h>


inline
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
  chashtable_hash_calc_t hash_calc_fn)
{
  assert(hashtable && allocator);
  assert(
    ((key_replicate_fn && key_cleanup_fn) || 
    (!key_replicate_fn && !key_cleanup_fn)) && 
    "if key_replicate_fn is provided, key_cleanup_fn must as well and vice" 
    "versa!");

  {
    hashtable->max_load_factor = max_load_factor;
    hashtable->allocator = allocator;
    hashtable->key_replicate = key_replicate_fn;
    hashtable->key_equal = key_equal_fn;
    hashtable->hash_calc = hash_calc_fn;

    cvector_setup(&hashtable->keys, key_size, 0, allocator, key_cleanup_fn);
    cvector_setup(&hashtable->values, elem_size, 0, allocator, elem_cleanup_fn);
    cvector_setup(&hashtable->indices, sizeof(uint32_t), 0, allocator, NULL);
  }
}

inline
void
chashtable_cleanup(chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));

  {
    cvector_cleanup(&hashtable->keys);
    cvector_cleanup(&hashtable->values);
    cvector_cleanup(&hashtable->indices);

    hashtable->max_load_factor = 0.f;
    hashtable->allocator = NULL;
    hashtable->key_replicate = NULL;
    hashtable->key_equal = NULL;
    hashtable->hash_calc = NULL;
  }
}

/** 
 * NOTE: will assert if 'src' is not initialized, or if 'dst' is initialized but
 * with non-zero size. type-critical member variables should also match.
 * NOTE: either 'dst' is def or 'allocator' is NULL, not both (we assert).
 */
inline
void 
chashtable_replicate(
  const chashtable_t *src, 
  chashtable_t *dst, 
  const allocator_t *allocator, 
  cvector_elem_replicate_t elem_replicate_fn)
{
  assert(src && !chashtable_is_def(src));
  assert(dst);

  assert(
    (chashtable_is_def(dst) && allocator) || 
    (
      !chashtable_is_def(dst) && 
      !allocator && 
      dst->indices.size == 0 && 
      dst->values.elem_size == src->values.elem_size && 
      dst->values.size == 0 && 
      dst->values.elem_cleanup == src->values.elem_cleanup && 
      dst->keys.elem_size == src->keys.elem_size && 
      dst->keys.size == 0 && 
      dst->keys.elem_cleanup == src->keys.elem_cleanup && 
      dst->key_replicate == src->key_replicate &&
      dst->key_equal == src->key_equal && 
      dst->hash_calc == src->hash_calc));

  if (chashtable_is_def(dst)) {
    chashtable_setup(
      dst, 
      src->keys.elem_size, 
      src->values.elem_size, 
      allocator, 
      src->max_load_factor, 
      src->keys.elem_cleanup, 
      src->key_replicate, 
      src->key_equal, 
      src->values.elem_cleanup, 
      src->hash_calc);
  } else {
    cvector_grow(&dst->indices, src->indices.capacity);
    cvector_grow(&dst->keys, src->keys.capacity);
    cvector_grow(&dst->values, src->values.capacity);
  }

  // the vector replicate rules should be satisfied by our earlier tests.
  cvector_replicate(&src->indices, &dst->indices, NULL, NULL);
  cvector_replicate(&src->keys, &dst->keys, NULL, dst->key_replicate);
  cvector_replicate(&src->values, &dst->values, NULL, elem_replicate_fn);

  dst->max_load_factor = src->max_load_factor;
}

inline
void
chashtable_fullswap(chashtable_t* src, chashtable_t* dst)
{
  assert(src && dst);
  {
    chashtable_t tmp = *src;
    *src = *dst;
    *dst = tmp;
  }
}

inline
size_t
chashtable_key_size(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->keys.elem_size;
}

inline
size_t
chashtable_value_size(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->values.elem_size;
}

inline
const allocator_t*
chashtable_allocator(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->allocator;
}

inline
chashtable_key_cleanup_t
chashtable_key_cleanup(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->keys.elem_cleanup;
}

inline
chashtable_key_replicate_t
chashtable_key_replicate(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->key_replicate;
}

inline
chashtable_key_equal_t
chashtable_key_equal(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->key_equal;
}

inline
chashtable_hash_calc_t
chashtable_hash_calc(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->hash_calc;
}

inline
chashtable_value_cleanup_t
chashtable_value_cleanup(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->values.elem_cleanup;
}

inline
int32_t
chashtable_empty(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return cvector_empty(&hashtable->values);
}

inline
size_t
chashtable_size(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return cvector_size(&hashtable->values);
}

inline
size_t
chashtable_capacity(const chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return cvector_capacity(&hashtable->values);
}

inline
void
chashtable_clear(chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  cvector_clear(&hashtable->values);
  cvector_clear(&hashtable->keys);
  cvector_clear(&hashtable->indices);
}

inline
float
chashtable_load_factor(chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return cvector_empty(&hashtable->values) ? 1.f :
    ((float)cvector_size(&hashtable->values) /
    (float)cvector_capacity(&hashtable->values));
}

inline
float
chashtable_max_load_factor(chashtable_t* hashtable)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  return hashtable->max_load_factor;
}

inline
void
chashtable_set_max_load_factor(
  chashtable_t* hashtable, 
  const float max_load_factor)
{
  assert(hashtable && !chashtable_is_def(hashtable));
  hashtable->max_load_factor = max_load_factor;
  chashtable_rehash(hashtable, chashtable_capacity(hashtable));
}

inline
void
chashtable_rehash(chashtable_t* hashtable, size_t count)
{
  assert(hashtable && !chashtable_is_def(hashtable));

  {
    size_t i, total;
    uint32_t hashed;
    size_t lower_bound = (size_t)ceilf(
      (float)chashtable_size(hashtable)/hashtable->max_load_factor);
    count = lower_bound > count ? lower_bound : count;

    // preserves but also allows the various vectors to shrink.
    cvector_grow(&hashtable->values, count);
    cvector_grow(&hashtable->keys, count);
    cvector_resize(&hashtable->indices, count);
    cvector_grow(&hashtable->indices, count);
    
    for (i = 0; i < count; ++i) 
      *cvector_as(&hashtable->indices, i, uint32_t) = CHASHTABLE_INVALID_INDEX;
    
    // iterate over the keys, hash them and set the index where it needs to be.
    for (i = 0, total = cvector_size(&hashtable->keys); i < total; ++i) {
      hashed = 
        (uint32_t)hashtable->hash_calc(cvector_at(&hashtable->keys, i)) % count;
      while (
        *cvector_as(&hashtable->indices, hashed, uint32_t) != 
        CHASHTABLE_INVALID_INDEX)
        hashed = (hashed + 1) % count;
      *cvector_as(&hashtable->indices, hashed, uint32_t) = i;
    }
  }
}

inline
void
chashtable_reserve(chashtable_t* hashtable, size_t count)
{
  assert(hashtable && !chashtable_is_def(hashtable));

  chashtable_rehash(
    hashtable, (size_t)ceilf((float)count/hashtable->max_load_factor));
}

#endif