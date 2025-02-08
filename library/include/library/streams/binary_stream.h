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
#include <library/internal/module.h>
#include <library/allocator/allocator.h>

#define STREAM_EOF ((size_t)-1)
#define STREAM_START_POS 0

#define STREAM_ALLOC_CHUNK 1024


////////////////////////////////////////////////////////////////////////////////
// TODO:
//  - support seeking, among other functionality.
// NOTE:
//  - we do not allocate until the first write request.
////////////////////////////////////////////////////////////////////////////////

typedef struct cvector_t cvector_t;

typedef
struct binary_stream_t {
  cvector_t *data;
  size_t pos;
  const allocator_t *allocator;
} binary_stream_t;

LIBRARY_API
void
binary_stream_def(binary_stream_t *stream);

LIBRARY_API
uint32_t
binary_stream_is_def(const binary_stream_t *stream);

// simply assigns the allocator, allocation is done on first write.
LIBRARY_API
void
binary_stream_setup(binary_stream_t *stream, const allocator_t *allocator);

LIBRARY_API
void
binary_stream_cleanup(binary_stream_t *stream);

LIBRARY_API
void
binary_stream_write(binary_stream_t *stream, const void *src, size_t length);

// return the number of bytes actually read and icrement pos.
LIBRARY_API
uint32_t
binary_stream_read(
  binary_stream_t *stream, 
  uint8_t buffer[], 
  size_t buffer_size, 
  uint32_t to_read);

#ifdef __cplusplus
}
#endif

#endif