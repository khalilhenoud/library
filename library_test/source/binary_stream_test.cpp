/**
 * @file binary_stream_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-12-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <library/allocator/allocator.h>
#include <library/streams/binary_stream.h>
#include <cstdint>
#include <cassert>
#include <common.h>


typedef 
struct {
  uint32_t i;
  uint32_t k;
  double l;
  uint8_t c;
} test_struct_t;

static
void
serialize_test(
  binary_stream_t *stream, 
  const test_struct_t &src, 
  const int32_t tabs)
{
  PRINT_FUNCTION;

  binary_stream_write(stream, &src.i, sizeof(uint32_t));
  binary_stream_write(stream, &src.k, sizeof(uint32_t));
  binary_stream_write(stream, &src.l, sizeof(double));
  binary_stream_write(stream, &src.c, sizeof(uint8_t));
}

static
void
deserialize_test(
  binary_stream_t *stream, 
  test_struct_t &dst, 
  const int32_t tabs)
{
  PRINT_FUNCTION;

  binary_stream_read(
    stream, (uint8_t *)&dst.i, sizeof(uint32_t), sizeof(uint32_t));
  binary_stream_read(
    stream, (uint8_t *)&dst.k, sizeof(uint32_t), sizeof(uint32_t));
  binary_stream_read(
    stream, (uint8_t *)&dst.l, sizeof(double), sizeof(double));
  binary_stream_read(
    stream, (uint8_t *)&dst.c, sizeof(uint8_t), sizeof(uint8_t));
}

static
void
print_test(const test_struct_t &src, const int32_t tabs)
{
  PRINT_FUNCTION;

  CTABS << src.i << ", " << src.k << ", " << src.l << ", " << src.c << 
    std::endl;
}

void
test_binarystream(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  binary_stream_t stream;
  binary_stream_def(&stream);
  binary_stream_setup(&stream, allocator);

  test_struct_t test;
  test.i = 702;
  test.k = test.i + 1;
  test.l = 66.5555;
  test.c = 'c';
  print_test(test, tabs + 1);

  serialize_test(&stream, test, tabs);

  test_struct_t to_read;
  deserialize_test(&stream, to_read, tabs);
  print_test(to_read, tabs + 1);

  assert(stream.pos == STREAM_EOF);

  binary_stream_cleanup(&stream);
}

void
test_binarystream_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_binarystream(allocator, tabs + 1);          NEWLINE;
}