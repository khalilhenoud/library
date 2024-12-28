/**
 * @file binary_stream.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_BINARY_STREAM_H
#define LIB_BINARY_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <library/containers/cvector.h>
#include <library/allocator/allocator.h>

#define STREAM_EOF ((size_t)-1)
#define STREAM_START_POS 0

#define STREAM_ALLOC_CHUNK 1024


// TODO: refine and expand this functionality considerably, right now it is very
// basic. this is by design as it is intended as a stop gap for now. This
// includes seeking, among other required functionality.
// Consider using a cvector for this, it makes more sense and it is already.
typedef
struct binary_stream_t {
  cvector_t data;
  size_t pos;
} binary_stream_t;

void
binary_stream_def(binary_stream_t *stream);

int32_t
binary_stream_is_def(const binary_stream_t *stream);

// simply assigns the allocator, allocation is done on first write.
void
binary_stream_setup(binary_stream_t *stream, const allocator_t *allocator);

void
binary_stream_cleanup(binary_stream_t *stream);

// TODO: support adding a range of elements into a cvector.
// always writes at the back of the stream.
void
binary_stream_write(binary_stream_t *stream, const void *src, size_t length);

// return the number of bytes actually read and icrement pos.
uint32_t
binary_stream_read(
  binary_stream_t *stream, 
  uint8_t buffer[], 
  size_t buffer_size, 
  uint32_t to_read);

#include "binary_stream.impl"

#ifdef __cplusplus
}
#endif

#endif