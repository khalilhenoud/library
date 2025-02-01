/**
 * @file chashmap.inl
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-09-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_HASHMAP_INL
#define LIB_HASHMAP_INL

#include <math.h>


/** 
 * NOTE: will assert if 'src' is not initialized, or if 'dst' is initialized but
 * with non-zero size. type-critical member variables should also match.
 * NOTE: either 'dst' is def or 'allocator' is NULL, not both (we assert).
 */
inline
void 
chashmap_replicate(
  const void *p_src, 
  void *p_dst, 
  const allocator_t *allocator)
{
  const chashmap_t *src = (const chashmap_t *)p_src;
  chashmap_t *dst = (chashmap_t *)p_dst;

  assert(src && !chashmap_is_def(src));
  assert(dst);

  assert(
    (chashmap_is_def(dst) && allocator) || 
    (
      !chashmap_is_def(dst) && 
      !allocator && 
      dst->indices.size == 0 &&
      dst->values.size == 0 && 
      elem_data_identical(&dst->values.elem_data, &src->values.elem_data) &&
      dst->keys.size == 0 && 
      elem_data_identical(&dst->keys.elem_data, &src->keys.elem_data)));

  if (chashmap_is_def(dst)) {
    chashmap_setup(
      dst, 
      get_type_data_from_elem_data(&src->keys.elem_data), 
      get_type_data_from_elem_data(&src->values.elem_data),
      allocator, 
      src->max_load_factor);
  } else {
    cvector_grow(&dst->indices, src->indices.capacity);
    cvector_grow(&dst->keys, src->keys.capacity);
    cvector_grow(&dst->values, src->values.capacity);
  }

  // the vector replicate rules should be satisfied by our earlier tests.
  cvector_replicate(&src->indices, &dst->indices, NULL);
  cvector_replicate(&src->keys, &dst->keys, NULL);
  cvector_replicate(&src->values, &dst->values, NULL);

  dst->max_load_factor = src->max_load_factor;
}

inline
void
chashmap_fullswap(void *lhs, void *rhs)
{
  assert(lhs && rhs);
  {
    chashmap_t *src = (chashmap_t *)lhs;
    chashmap_t *dst = (chashmap_t *)rhs;
    chashmap_t tmp = *src;
    *src = *dst;
    *dst = tmp;
  }
}

inline
void
chashmap_cleanup(void *p_hashmap, const allocator_t *allocator)
{
  chashmap_t *hashmap = (chashmap_t *)p_hashmap;
  assert(hashmap && !chashmap_is_def(hashmap));
  assert(allocator == NULL);

  {
    cvector_cleanup(&hashmap->keys, NULL);
    cvector_cleanup(&hashmap->values, NULL);
    cvector_cleanup(&hashmap->indices, NULL);

    hashmap->max_load_factor = 0.f;
    hashmap->allocator = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////
inline
void
chashmap_setup(
  chashmap_t *hashmap, 
  type_data_t key_type_data,
  type_data_t value_type_data,
  const allocator_t *allocator,
  const float max_load_factor)
{
  assert(hashmap && allocator);

  {
    hashmap->max_load_factor = max_load_factor;
    hashmap->allocator = allocator;

    cvector_setup(&hashmap->keys, key_type_data, 0, allocator);
    cvector_setup(&hashmap->values, value_type_data, 0, allocator);
    cvector_setup(&hashmap->indices, get_type_data(uint32_t), 0, allocator);

    assert(
      hashmap->keys.elem_data.vtable && 
      hashmap->keys.elem_data.vtable->fn_hash &&
      "key hash function must exist!");

    assert(
      ((
        hashmap->keys.elem_data.vtable->fn_replicate && 
        hashmap->keys.elem_data.vtable->fn_cleanup) || (
        !hashmap->keys.elem_data.vtable->fn_replicate && 
        !hashmap->keys.elem_data.vtable->fn_cleanup)) &&
        "if key_replicate_fn is provided, key_cleanup_fn must as well and vice"
        "versa!");
  }
}

inline
size_t
chashmap_key_size(const chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  return hashmap->keys.elem_data.size;
}

inline
size_t
chashmap_value_size(const chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  return hashmap->values.elem_data.size;
}

inline
fn_cleanup_t
chashmap_key_cleanup(const chashmap_t* hashmap)
{
  return elem_data_get_cleanup_fn(&hashmap->keys.elem_data);
}

inline
fn_replicate_t
chashmap_key_replicate(const chashmap_t* hashmap)
{
  return elem_data_get_replicate_fn(&hashmap->keys.elem_data);
}

inline
fn_is_equal_t
chashmap_key_equal(const chashmap_t* hashmap)
{
  return elem_data_get_is_equal_fn(&hashmap->keys.elem_data);
}

inline
fn_hash_t
chashmap_hash_calc(const chashmap_t* hashmap)
{
  return elem_data_get_hash_fn(&hashmap->keys.elem_data);
}

inline
int32_t
chashmap_empty(const chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  return cvector_empty(&hashmap->values);
}

inline
size_t
chashmap_size(const chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  return cvector_size(&hashmap->values);
}

inline
size_t
chashmap_capacity(const chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  return cvector_capacity(&hashmap->values);
}

inline
void
chashmap_clear(chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  cvector_clear(&hashmap->values);
  cvector_clear(&hashmap->keys);
  cvector_clear(&hashmap->indices);
}

inline
float
chashmap_load_factor(chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  return cvector_empty(&hashmap->values) ? 1.f :
    ((float)cvector_size(&hashmap->values) /
    (float)cvector_capacity(&hashmap->values));
}

inline
float
chashmap_max_load_factor(chashmap_t* hashmap)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  return hashmap->max_load_factor;
}

inline
void
chashmap_set_max_load_factor(
  chashmap_t* hashmap, 
  const float max_load_factor)
{
  assert(hashmap && !chashmap_is_def(hashmap));
  hashmap->max_load_factor = max_load_factor;
  chashmap_rehash(hashmap, chashmap_capacity(hashmap));
}

inline
void
chashmap_rehash(chashmap_t* hashmap, size_t count)
{
  assert(hashmap && !chashmap_is_def(hashmap));

  {
    size_t i, total;
    uint32_t hashed;
    size_t lower_bound = (size_t)ceilf(
      (float)chashmap_size(hashmap)/hashmap->max_load_factor);
    count = lower_bound > count ? lower_bound : count;

    // preserves but also allows the various vectors to shrink.
    cvector_grow(&hashmap->values, count);
    cvector_grow(&hashmap->keys, count);
    cvector_resize(&hashmap->indices, count);
    cvector_grow(&hashmap->indices, count);
    
    for (i = 0; i < count; ++i) 
      *cvector_as(&hashmap->indices, i, uint32_t) = CHASHTABLE_INVALID_INDEX;
    
    // iterate over the keys, hash them and set the index where it needs to be.
    for (i = 0, total = cvector_size(&hashmap->keys); i < total; ++i) {
      hashed = (uint32_t)(chashmap_hash_calc(hashmap)(
        cvector_at(&hashmap->keys, i)) % count);
      while (
        *cvector_as(&hashmap->indices, hashed, uint32_t) != 
        CHASHTABLE_INVALID_INDEX)
        hashed = (hashed + 1) % count;
      *cvector_as(&hashmap->indices, hashed, uint32_t) = i;
    }
  }
}

inline
void
chashmap_reserve(chashmap_t* hashmap, size_t count)
{
  assert(hashmap && !chashmap_is_def(hashmap));

  chashmap_rehash(
    hashmap, (size_t)ceilf((float)count/hashmap->max_load_factor));
}

inline
chashmap_iterator_t
chashmap_begin(chashmap_t* map)
{
  assert(map);
  {
    chashmap_iterator_t iter;
    iter.map = map;
    iter.index = 0;
    return iter;
  }
}

inline
chashmap_iterator_t
chashmap_end(chashmap_t* map)
{
  assert(map);
  {
    chashmap_iterator_t iter;
    iter.map = map;
    iter.index = chashmap_size(map);
    return iter;
  }
}

inline
void
chashmap_advance(chashmap_iterator_t* iter)
{
  assert(iter);
  assert(iter->map && "the iterator is invalid!");
  ++iter->index;
}

inline
int32_t
chashmap_iter_equal(chashmap_iterator_t left, chashmap_iterator_t right)
{
  return left.map == right.map && left.index == right.index;
}

#endif