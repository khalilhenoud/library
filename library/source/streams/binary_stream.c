/**
 * @file binary_stream.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <assert.h>
#include <string.h>
#include <library/containers/cvector.h>


void
binary_stream_def(binary_stream_t *stream)
{
  assert(stream);

  stream->data = NULL; 
  stream->pos = STREAM_START_POS;
  stream->allocator = NULL;
}

uint32_t
binary_stream_is_def(const binary_stream_t *stream)
{
  assert(stream);
  return 
    stream->data == NULL && 
    stream->pos == STREAM_START_POS && 
    stream->allocator == NULL;
}

void
binary_stream_setup(binary_stream_t *stream, const allocator_t *allocator)
{
  assert(stream && allocator && binary_stream_is_def(stream));
  stream->allocator = allocator;
  stream->data = (cvector_t*)allocator->mem_alloc(sizeof(cvector_t));
  cvector_def(stream->data);
  cvector_setup(
    stream->data, 
    get_type_data(uint8_t), STREAM_ALLOC_CHUNK, allocator);
}

void
binary_stream_cleanup(binary_stream_t *stream)
{
  assert(stream && !binary_stream_is_def(stream));
  cvector_cleanup(stream->data, NULL);
  stream->allocator->mem_free(stream->data);
  stream->pos = STREAM_START_POS;
  stream->allocator = NULL;
}

void
binary_stream_write(
  binary_stream_t *stream, 
  const void *src, 
  size_t length)
{
  assert(stream && !binary_stream_is_def(stream));
  assert(src && length);

  {
    // check the remaining number of bytes.
    int64_t available = stream->data->capacity - stream->data->size;
    uint32_t insert_at = stream->data->size;
    if (length > available) {
      size_t grow_by = 
        ((length - available) / STREAM_ALLOC_CHUNK + 1) * STREAM_ALLOC_CHUNK;
      cvector_reserve(stream->data, stream->data->capacity + grow_by);
    }
    cvector_resize(stream->data, insert_at + length);

    {
      // the casts here is only necessary in msvc.
      const uint8_t *src_data = (const uint8_t *)src;
      uint8_t *dst_data = (uint8_t *)stream->data->data; 
      dst_data += insert_at;
      do {
        *dst_data++ = *src_data++;
      } while (--length);
    }
  }
}

uint32_t
binary_stream_read(
  binary_stream_t *stream, 
  uint8_t buffer[], 
  size_t buffer_size, 
  uint32_t to_read)
{
  assert(stream && !binary_stream_is_def(stream));
  assert(buffer && buffer_size && to_read && to_read <= buffer_size);
  assert(stream->pos < stream->data->size);

  if (stream->pos != STREAM_EOF) {
    const uint8_t *src = (uint8_t *)stream->data->data + stream->pos; 
    int64_t available = stream->data->size - stream->pos;
    to_read = available < to_read ? available : to_read;
    memcpy(buffer, src, to_read);
    stream->pos += to_read;
    stream->pos = 
      (stream->pos == stream->data->size) ? STREAM_EOF : stream->pos;
    return to_read;
  }

  return STREAM_EOF;
}