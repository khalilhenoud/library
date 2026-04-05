/**
 * @file asset_ref.c
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2026-04-04
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <library/asset/asset_ref.h>
#include <library/core/core.h>
#include <library/streams/binary_stream.h>
#include <library/type_registry/type_registry.h>


void
asset_ref_serialize(
  const void *src,
  binary_stream_t *stream)
{
  assert(src && stream);

  {
    const asset_ref_t *ref = src;
    cstring_serialize(&ref->path, stream);
    binary_stream_write(stream, &ref->type_id, sizeof(uint32_t));
  }
}

void
asset_ref_deserialize(
  void *dst,
  const allocator_t *allocator,
  binary_stream_t *stream)
{
  assert(dst && allocator && stream);

  {
    asset_ref_t *ref = dst;
    cstring_def(&ref->path);
    cstring_deserialize(&ref->path, allocator, stream);
    binary_stream_read(
      stream,
      (uint8_t *)&ref->type_id,
      sizeof(uint32_t),
      sizeof(uint32_t));
  }
}

void
asset_ref_cleanup(void *ptr, const allocator_t *allocator)
{
  assert(ptr && allocator);

  {
    asset_ref_t *ref = ptr;
    cstring_cleanup2(&ref->path);
    memset(ref, 0, sizeof(asset_ref_t));
  }
}

////////////////////////////////////////////////////////////////////////////////
INITIALIZER(register_asset_ref_t)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = asset_ref_def;
  vtable.fn_is_def = asset_ref_is_def;
  vtable.fn_replicate = asset_ref_replicate;
  vtable.fn_fullswap = asset_ref_fullswap;
  vtable.fn_serialize = asset_ref_serialize;
  vtable.fn_deserialize = asset_ref_deserialize;
  vtable.fn_hash = asset_ref_hash;
  vtable.fn_is_equal = asset_ref_is_equal;
  vtable.fn_type_size = asset_ref_type_size;
  vtable.fn_cleanup = asset_ref_cleanup;
  register_type(get_type_id(asset_ref_t), &vtable);
}