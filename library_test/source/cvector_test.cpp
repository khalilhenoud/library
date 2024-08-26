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
print_meta(cvector_t& vec, const int32_t tabs) 
{
  CTABS << PRINT_BOOL(cvector_empty(&vec)) << std::endl;
  CTABS << "vec.size = " << cvector_size(&vec) << std::endl;
  CTABS << "vec.elem_size = " << cvector_elem_size(&vec) << std::endl;
  CTABS << "vec.capacity = " << cvector_capacity(&vec) << std::endl;
  CTABS << "vec.allocator = " << cvector_allocator(&vec) << std::endl;
  CTABS << "vec.elem_cleanup = " << cvector_elem_cleanup(&vec) << std::endl;
};

void
test_cvector_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cvector_t vec = cvector_def();
  print_cvector(vec, tabs + 1);
  CTABS << PRINT_BOOL(cvector_is_def(&vec)) << std::endl;
}

void
test_cvector_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cvector_t vec_i8 = cvector_def();
  cvector_setup(&vec_i8, sizeof(char), 4, allocator, NULL);
  print_meta(vec_i8, tabs + 1);
  for (int32_t i = 100; i < 120; ++i)
    cvector_push_back(&vec_i8, (char)i + 1, char);
  print_cvector(vec_i8, tabs + 1);
  print_cvector_content<char>(vec_i8, tabs + 1);
  cvector_cleanup(&vec_i8);
  NEWLINE;

  cvector_t vec_i16 = cvector_def();
  cvector_setup(&vec_i16, sizeof(short), 4, allocator, NULL);
  print_meta(vec_i16, tabs + 1);
  for (int32_t i = 120; i < 140; ++i)
    cvector_push_back(&vec_i16, (short)i + 1, short);
  print_cvector(vec_i16, tabs + 1);
  print_cvector_content<short>(vec_i16, tabs + 1);
  cvector_cleanup(&vec_i16);
  NEWLINE;

  cvector_t vec_i32 = cvector_def();
  cvector_setup(&vec_i32, sizeof(int32_t), 4, allocator, NULL);
  print_meta(vec_i32, tabs + 1);
  for (int32_t i = 0; i < 10; ++i)
    cvector_push_back(&vec_i32, (int32_t)i + 1, int32_t);
  print_cvector(vec_i32, tabs + 1);
  print_cvector_content<int32_t>(vec_i32, tabs + 1);
  cvector_cleanup(&vec_i32);
  NEWLINE;

  cvector_t vec_f32 = cvector_def();
  cvector_setup(&vec_f32, sizeof(float), 4, allocator, NULL);
  print_meta(vec_f32, tabs + 1);
  for (int32_t i = 1; i < 20; ++i)
    cvector_push_back(&vec_f32, (float)i + 1.75f * i, float);
  print_cvector(vec_f32, tabs + 1);
  print_cvector_content<float>(vec_f32, tabs + 1);
  cvector_cleanup(&vec_f32);
  NEWLINE;

  cvector_t vec_f64 = cvector_def();
  cvector_setup(&vec_f64, sizeof(double), 4, allocator, NULL);
  print_meta(vec_f64, tabs + 1);
  for (int32_t i = 20; i < 35; ++i)
    cvector_push_back(&vec_f64, (double)i + 1.75 * i, double);
  print_cvector(vec_f64, tabs + 1);
  print_cvector_content<double>(vec_f64, tabs + 1);
  cvector_cleanup(&vec_f64);
}

void
test_cvector_iterators(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cvector_t vec = cvector_def();
  cvector_setup(&vec, sizeof(double), 16, allocator, NULL);
  print_meta(vec, tabs + 1);
  for (int32_t i = 0; i < 5; ++i)
    cvector_push_back(&vec, (double)i + 1.75 * i, double);

  CTABS << "values: ";
  for (
    cvector_iterator(double) iter = cvector_begin(&vec, double); 
    iter != cvector_end(&vec, double); 
    ++iter) {
      std::cout << *iter << " "; 
    }
  NEWLINE;
  CTABS << "front: " << *cvector_front(&vec, double) << std::endl;
  CTABS << "back : " << *cvector_back(&vec, double) << std::endl;

  CTABS << "cvector_shrink_to_fit(&vec)" << std::endl;
  cvector_shrink_to_fit(&vec);
  print_meta(vec, tabs + 1);

  while (!cvector_empty(&vec)) {
    print_cvector_content<double>(vec, tabs + 1);
    cvector_pop_back(&vec);
  }
  
  cvector_cleanup(&vec);
}

void
test_cvector_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
}

void
test_cvector_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_cvector_def(allocator, tabs + 1);          NEWLINE;
  test_cvector_basics(allocator, tabs + 1);       NEWLINE;
  test_cvector_iterators(allocator, tabs + 1);    NEWLINE;
  test_cvector_ops(allocator, tabs + 1);          NEWLINE;
}