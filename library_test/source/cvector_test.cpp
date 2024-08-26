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

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif

#define TABS std::string(tabs, '\t')
#define CTABS std::cout << TABS
#define PRINT_FUNCTION CTABS << "[" __FUNCTION_NAME__ "]"  << std::endl
#define PRINT_BOOL(x) #x << " = " << (x ? "true" : "false")
#define PRINT(x) #x << " = " << x
#define NEWLINE std::cout << std::endl


template<typename T>
void
print_cvector_content(cvector_t& vec, const int32_t tabs)
{
  PRINT_FUNCTION;
  CTABS << "vec.size = " << vec.size << std::endl;
  CTABS << "vec.elem_size = " << vec.elem_size << std::endl;
  CTABS << "vec.capacity = " << vec.capacity << std::endl;
  CTABS << "vec.allocator = " << vec.allocator << std::endl;
  CTABS << "vec.elem_cleanup = " << vec.elem_cleanup << std::endl;
  CTABS << "vec.ptr = " << vec.ptr << std::endl;

  CTABS << "vec.ptr: ";
  for (size_t i = 0; i < vec.size; ++i)
    std::cout << *cvector_as(&vec, i, T) << " ";
  NEWLINE;
}

void
print_cvector(cvector_t& vec, const int32_t tabs)
{
  PRINT_FUNCTION;
  CTABS << "vec.size = " << vec.size << std::endl;
  CTABS << "vec.elem_size = " << vec.elem_size << std::endl;
  CTABS << "vec.capacity = " << vec.capacity << std::endl;
  CTABS << "vec.allocator = " << vec.allocator << std::endl;
  CTABS << "vec.elem_cleanup = " << vec.elem_cleanup << std::endl;
  CTABS << "vec.ptr = " << vec.ptr << std::endl;
}

void
test_cvector_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cvector_t vec = cvector_def();
  print_cvector(vec, tabs + 1);
  CTABS << PRINT_BOOL(cvector_is_def(&vec)) << std::endl;
}

void
test_cvector_basic(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cvector_t vec = cvector_def();
  cvector_setup(&vec, sizeof(int32_t), 4, allocator, NULL);
  CTABS << PRINT_BOOL(cvector_empty(&vec)) << std::endl;
  CTABS << "vec.size = " << cvector_size(&vec) << std::endl;
  CTABS << "vec.elem_size = " << cvector_elem_size(&vec) << std::endl;
  CTABS << "vec.capacity = " << cvector_capacity(&vec) << std::endl;
  CTABS << "vec.allocator = " << cvector_allocator(&vec) << std::endl;
  CTABS << "vec.elem_cleanup = " << cvector_elem_cleanup(&vec) << std::endl;
  for (int32_t i = 0; i < 10; ++i)
    cvector_push_back(&vec, i + 1, int32_t);
  print_cvector_content<int32_t>(vec, tabs + 1);

  cvector_cleanup(&vec);
}

void
test_cvector_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_cvector_def(allocator, tabs + 1);        NEWLINE;
  test_cvector_basic(allocator, tabs + 1);      NEWLINE;
}