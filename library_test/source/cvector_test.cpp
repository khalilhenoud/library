/**
 * @file cvector_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-08-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <library/allocator/allocator.h>
#include <library/containers/cvector.h>
#include <stdint.h>
#include <cassert>
#include <iostream>


/*
- [ ]  cvector
- [ ]  cvector_empty
- [ ]  cvector_capacity
- [ ]  cvector_size
- [ ]  cvector_init
- [ ]  cvector_reserve
- [ ]  cvector_erase
- [ ]  cvector_clear
- [ ]  cvector_free
- [ ]  cvector_begin
- [ ]  cvector_end
- [ ]  cvector_compute_next_grow
- [ ]  cvector_push_back
- [ ]  cvector_set_size
- [ ]  cvector_set_capacity
- [ ]  cvector_set_elem_destructor
- [ ]  cvector_shrink_to_fit
- [ ]  cvector_at
- [ ]  cvector_front
- [ ]  cvector_back
- [ ]  cvector_pop_back
- [ ]  cvector_copy
- [ ]  cvector_swap
- [ ]  cvector_insert
- [ ]  cvector_grow
- [ ]  cvector_resize
- [ ]  cvector_for_each_in
*/

void
test_cvector_main(
  const allocator_t* allocator)
{
  std::cout << "test_cvector_main()" << std::endl;
}