/**
 * @file default_allocator_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2025-10-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <cassert>
#include <cstdint>
#include <vector>
#include <common.h>
#include <library/allocator/allocator.h>
#include <library/memory/memory.h>


static std::vector<uintptr_t> allocated;

static
void
alloc_callback(void *block)
{
  allocated.push_back(uintptr_t(block));
}

static
void
realloc_callback(void *old_block, void *block)
{
  allocated.erase(
    std::remove_if(
      allocated.begin(),
      allocated.end(),
      [=](uintptr_t elem) { return (uintptr_t)old_block == elem; }),
    allocated.end());
  allocated.push_back(uintptr_t(block));
}

static
void
free_callback(void *block)
{
  allocated.erase(
    std::remove_if(
      allocated.begin(),
      allocated.end(),
      [=](uintptr_t elem) { return (uintptr_t)block == elem; }),
    allocated.end());
}

void
test_cvector_main(const allocator_t *allocator, const int32_t tabs = 0);

void
test_default_allocator_main(const int32_t tabs)
{
  PRINT_FUNCTION;

  set_alloc_callback(alloc_callback);
  set_realloc_callback(realloc_callback);
  set_free_callback(free_callback);

  // we resuse the vector unit test but with the default allocator in place
  test_cvector_main(&g_default_allocator, tabs + 1);

  set_alloc_callback(NULL);
  set_realloc_callback(NULL);
  set_free_callback(NULL);

  std::cout << "default memory allocations remaining: " << allocated.size();
  std::cout << std::endl;
  assert(allocated.size() == 0);
}