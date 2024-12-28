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


void
test_binarystream_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  // chashtable_t map = chashtable_def();
  // print_meta(map, tabs);
  // CTABS << PRINT_BOOL(chashtable_is_def(&map)) << std::endl;
}

void
test_binarystream_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_binarystream_def(allocator, tabs + 1);          NEWLINE;
}