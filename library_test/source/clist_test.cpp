/**
 * @file clist_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-08-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <library/allocator/allocator.h>
#include <library/containers/clist.h>
#include <stdint.h>
#include <cassert>
#include <iostream>

void
test_clist_main(const allocator_t* allocator, const int32_t tabs)
{
  clist_t list = clist_def();
  std::cout << clist_is_def(&list) << std::endl;
}