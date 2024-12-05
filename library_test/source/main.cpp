/**
 * @file main.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-07-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <library/allocator/allocator.h>
#include <malloc.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>


std::vector<uintptr_t> allocated;

void* 
allocate(size_t size)
{
  void* block = malloc(size);
  allocated.push_back(uintptr_t(block));
  return block;
}

void* 
reallocate(void* old_block, size_t size)
{
  void* block = realloc(old_block, size);
  assert(block);
  allocated.erase(
    std::remove_if(
      allocated.begin(), 
      allocated.end(), 
      [=](uintptr_t elem) { return (uintptr_t)old_block == elem; }), 
    allocated.end());
  allocated.push_back(uintptr_t(block));
  return block;
}

void* 
container_allocate(size_t count, size_t elem_size)
{
  void* block = calloc(count, elem_size);
  allocated.push_back(uintptr_t(block));
  return block;
}

void 
free_block(void* block)
{
  allocated.erase(
    std::remove_if(
      allocated.begin(), 
      allocated.end(), 
      [=](uintptr_t elem) { return (uintptr_t)block == elem; }), 
    allocated.end());
  free(block);
}

void
test_cvector_main(const allocator_t* allocator, const int32_t tabs = 0);

void
test_clist_main(const allocator_t* allocator, const int32_t tabs = 0);

void
test_hash_main(const allocator_t* allocator, const int32_t tabs = 0);

void
test_chashtable_main(const allocator_t* allocator, const int32_t tabs = 0);

int 
main(int argc, char *argv[])
{
  allocator_t allocator;
  allocator.mem_alloc = allocate;
  allocator.mem_cont_alloc = container_allocate;
  allocator.mem_free = free_block;
  allocator.mem_alloc_alligned = NULL;
  allocator.mem_realloc = reallocate;

  test_cvector_main(&allocator);
  test_clist_main(&allocator);
  test_hash_main(&allocator);
  test_chashtable_main(&allocator);

  std::cout << "allocation remaining: " << allocated.size() << std::endl;
  assert(allocated.size() == 0);

  return 0;
}
