/**
 * @file asset_ref.h
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2026-04-04
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef C_LIBRARY_ASSET_REF
#define C_LIBRARY_ASSET_REF

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <library/internal/module.h>
#include <library/allocator/allocator.h>
#include <library/string/cstring.h>


////////////////////////////////////////////////////////////////////////////////
//| asset_ref_t, '*' = asset_ref
//|=============================================================================
//| OPERATION                   | SUPPORTED
//|=============================================================================
//|    *_def                    | YES
//|    *_is_def                 | YES
//|    *_replicate              | YES
//|    *_fullswap               | YES
//|    *_serialize              | YES
//|    *_deserialize            | YES
//|    *_hash                   | YES
//|    *_is_equal               | YES
//|    *_type_size              | YES
//|    *_type_alignment         |
//|    *_type_id_count          |
//|    *_type_ids               |
//|    *_owns_alloc             |
//|    *_get_alloc              |
//|    *_cleanup                | YES
//|    *_get_dir                |
//|    *_get_loader             |
//|    *_get_deloader           |
//|    *_type_asset_count       |
//|    *_type_get_assets        |
//|    *_is_asset_type          |
////////////////////////////////////////////////////////////////////////////////

typedef struct binary_stream_t binary_stream_t;

// NOTE: type_id is a bit redundant since we can get the type by checking the
// parent directory of the asset. Still, it is useful for speedup, and for error
// checking. For the time being it stays.
typedef
struct asset_ref_t {
  cstring_t path;
  uint32_t type_id;
} asset_ref_t;

inline
void
asset_ref_def(void *ptr)
{
  assert(ptr);

  {
    asset_ref_t *ref = ptr;
    memset(ref, 0, sizeof(asset_ref_t));
  }
}

inline
uint32_t
asset_ref_is_def(const void *ptr)
{
  assert(ptr);

  {
    const asset_ref_t *ref = ptr;
    asset_ref_t def;
    asset_ref_def(&def);
    return !memcmp(ref, &def, sizeof(asset_ref_t));
  }
}

inline
void
asset_ref_replicate(
  const void *_src,
  void *_dst,
  const allocator_t *allocator)
{
  const asset_ref_t *src = _src;
  asset_ref_t *dst = _dst;

  assert(src && !asset_ref_is_def(src));
  assert(dst && asset_ref_is_def(dst));

  // use the default allocator if non-exists
  allocator = allocator == NULL ? &g_default_allocator : allocator;
  dst->type_id = src->type_id;
  cstring_setup(&dst->path, src->path.str, allocator);
}

inline
void
asset_ref_fullswap(void *_lhs, void *_rhs)
{
  asset_ref_t *lhs = _lhs;
  asset_ref_t *rhs = _rhs;

  assert(lhs && rhs);
  cstring_fullswap(&lhs->path, &rhs->path);
  {
    uint32_t value = lhs->type_id;
    lhs->type_id = rhs->type_id;
    rhs->type_id = value;
  }
}

LIBRARY_API
void
asset_ref_serialize(
  const void *src,
  binary_stream_t *stream);

LIBRARY_API
void
asset_ref_deserialize(
  void *dst,
  const allocator_t *allocator,
  binary_stream_t *stream);

inline
uint32_t
asset_ref_hash(const void *ptr)
{
  const asset_ref_t *ref = ptr;
  assert(ptr);
  return cstring_hash(&ref->path);
}

inline
uint32_t
asset_ref_is_equal(
  const void *_lhs,
  const void *_rhs)
{
  const asset_ref_t *lhs = _lhs;
  const asset_ref_t *rhs = _rhs;
  assert(lhs && rhs);
  return
    cstring_is_equal(&lhs->path, &rhs->path) && lhs->type_id == rhs->type_id;
}

inline
size_t
asset_ref_type_size(void)
{
  return sizeof(asset_ref_t);
}

LIBRARY_API
void
asset_ref_cleanup(void *ptr, const allocator_t *allocator);

inline
void
asset_ref_sanity_check(const asset_ref_t *asset_ref, uint32_t type_id)
{
  assert(asset_ref->type_id == type_id);
}

#ifdef __cplusplus
}
#endif

#endif