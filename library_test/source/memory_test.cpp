/**
 * @file memory_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2025-10-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <cassert>
#include <cstdint>
#include <vector>
#include <common.h>
#include <library/memory/memory.h>


static std::vector<uintptr_t> allocated;

static
void
alloc_callback(void *block)
{
  std::cout << "allocated: " << block << std::endl;
  allocated.push_back(uintptr_t(block));
}

static
void
realloc_callback(void *old_block, void *block)
{
  std::cout << "re_allocated: " << old_block << ", to: " << block << std::endl;
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
  std::cout << "freed: " << block << std::endl;
  allocated.erase(
    std::remove_if(
      allocated.begin(),
      allocated.end(),
      [=](uintptr_t elem) { return (uintptr_t)block == elem; }),
    allocated.end());
}

void
test_memory_main(const int32_t tabs)
{
  PRINT_FUNCTION;

  set_alloc_callback(alloc_callback);
  set_realloc_callback(realloc_callback);
  set_free_callback(free_callback);

  uint32_t *ptr = (uint32_t *)mem_alloc(sizeof(uint32_t));
  mem_free(ptr);

  ptr = (uint32_t *)mem_cont_alloc(30, sizeof(uint32_t));
  mem_free(ptr);

  ptr = (uint32_t *)mem_alloc(sizeof(uint32_t) * 5);
  ptr = (uint32_t *)mem_realloc(ptr, sizeof(uint32_t) * 15);
  mem_free(ptr);

  set_alloc_callback(NULL);
  set_realloc_callback(NULL);
  set_free_callback(NULL);

  std::cout << "c memory allocations remaining: " << allocated.size();
  std::cout << std::endl;
  assert(allocated.size() == 0);
}