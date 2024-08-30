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

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif

#define TABS std::string(tabs, '\t')
#define CTABS std::cout << TABS
#define PRINT_FUNCTION CTABS << "-------[" __FUNCTION_NAME__ "]-------" \
  << std::endl
#define PRINT_DESC(x) CTABS << ">>>>>>> " << x << " <<<<<<<" << std::endl
#define PRINT_BOOL(x) #x << " = " << (x ? "true" : "false")
#define PRINT(x) #x << " = " << x
#define NEWLINE std::cout << std::endl


template<typename T>
void
print_clist_content(clist_t& list, const int32_t tabs)
{
  CTABS << "values: ";
  for (size_t i = 0; i < list.size; ++i)
    std::cout << *clist_as(&list, i, T) << " ";
  NEWLINE;
}

static
void
print_meta(clist_t& list, const int32_t tabs) 
{
  CTABS << 
  "list(size: " << list.size << 
  ", elem_size: " << list.elem_size <<
  ", allocator: " << (uint64_t)(list.allocator) <<
  ", elem_cleanup: " << (uint64_t)(list.elem_cleanup) << 
  ", nodes: " << (uint64_t)(list.nodes) << ")" << 
  std::endl;
};

void
test_clist_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  clist_t list = clist_def();
  print_meta(list, tabs);
  CTABS << PRINT_BOOL(clist_is_def(&list)) << std::endl;
}

void
test_clist_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("defines and populate basic types of lists");

  clist_t list_i8 = clist_def();
  clist_setup(&list_i8, sizeof(char), allocator, NULL);
  print_meta(list_i8, tabs);
  for (int32_t i = 100; i < 120; ++i)
    clist_push_back(&list_i8, (char)i + 1, char);
  print_meta(list_i8, tabs);
  print_clist_content<char>(list_i8, tabs);
  clist_cleanup(&list_i8);
  NEWLINE;

  clist_t list_i16 = clist_def();
  clist_setup(&list_i16, sizeof(short), allocator, NULL);
  print_meta(list_i16, tabs);
  for (int32_t i = 120; i < 140; ++i)
    clist_push_back(&list_i16, (short)i + 1, short);
  print_meta(list_i16, tabs);
  print_clist_content<short>(list_i16, tabs);
  clist_cleanup(&list_i16);
  NEWLINE;

  clist_t list_i32 = clist_def();
  clist_setup(&list_i32, sizeof(int32_t), allocator, NULL);
  print_meta(list_i32, tabs);
  for (int32_t i = 0; i < 10; ++i)
    clist_push_back(&list_i32, (int32_t)i + 1, int32_t);
  print_meta(list_i32, tabs);
  print_clist_content<int32_t>(list_i32, tabs);
  clist_cleanup(&list_i32);
  NEWLINE;

  clist_t list_f32 = clist_def();
  clist_setup(&list_f32, sizeof(float), allocator, NULL);
  print_meta(list_f32, tabs);
  for (int32_t i = 1; i < 20; ++i)
    clist_push_back(&list_f32, (float)i + 1.75f * i, float);
  print_meta(list_f32, tabs);
  print_clist_content<float>(list_f32, tabs);
  clist_cleanup(&list_f32);
  NEWLINE;

  clist_t list_f64 = clist_def();
  clist_setup(&list_f64, sizeof(double), allocator, NULL);
  print_meta(list_f64, tabs);
  for (int32_t i = 20; i < 35; ++i)
    clist_push_back(&list_f64, (double)i + 1.75 * i, double);
  print_meta(list_f64, tabs);
  print_clist_content<double>(list_f64, tabs);
  clist_cleanup(&list_f64);
}

void
test_clist_iterators(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("tests for iterators, front, back, push_back functions...");

  clist_t list = clist_def();
  clist_setup(&list, sizeof(double), allocator, NULL);
  print_meta(list, tabs);
  for (int32_t i = 0; i < 5; ++i)
    clist_push_back(&list, (double)i + 1.75 * i, double);

  CTABS << "values: ";
  for (
    clist_iterator iter = clist_begin(&list); 
    !clist_iter_equal(iter, clist_end(&list)); 
    clist_advance(&iter)) {
      std::cout << *clist_deref(&iter, double) << " "; 
    }
  NEWLINE;
  CTABS << "front: " << *clist_front(&list, double) << std::endl;
  CTABS << "back: " << *clist_back(&list, double) << std::endl;

  CTABS << "clist_pop_back() loop: " << std::endl;
  while (!clist_empty(&list)) {
    print_clist_content<double>(list, tabs);
    clist_pop_back(&list);
  }

  clist_cleanup(&list);
}

void
test_clist_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("replicate and fullswap function testing");

  {
    CTABS << "replicate testing: " << std::endl;
    // replicate
    clist_t left = clist_def();
    clist_setup(&left, sizeof(int32_t), allocator, NULL);
    for (int32_t i = 0; i < 10; ++i)
      clist_push_back(&left, i + 1, int32_t);
    print_meta(left, tabs);
    print_clist_content<int32_t>(left, tabs);

    clist_t right = clist_def();
    clist_replicate(&left, &right, allocator, NULL);
    print_clist_content<int32_t>(right, tabs);
    clist_cleanup(&right);

    clist_setup(&right, sizeof(int32_t), allocator, NULL);
    clist_replicate(&left, &right, NULL, NULL);
    print_clist_content<int32_t>(right, tabs);
    print_meta(right, tabs);
    
    clist_cleanup(&right);
    clist_cleanup(&left);
  }

  {
    CTABS << "fullswap testing: " << std::endl;
    // fullswap
    clist_t left = clist_def();
    clist_setup(&left, sizeof(int32_t), allocator, NULL);
    for (int32_t i = 10; i < 15; ++i)
      clist_push_back(&left, i + 1, int32_t);

    clist_t right = clist_def();
    clist_setup(&right, sizeof(int32_t), allocator, NULL);
    for (int32_t i = 120; i < 130; ++i)
      clist_push_back(&right, i + 1, int32_t);
    
    print_clist_content<int32_t>(left, tabs);
    print_clist_content<int32_t>(right, tabs);

    clist_fullswap(&left, &right);

    print_clist_content<int32_t>(left, tabs);
    print_clist_content<int32_t>(right, tabs);

    clist_cleanup(&right);
    clist_cleanup(&left);
  }
}

void
test_clist_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_clist_def(allocator, tabs + 1);          NEWLINE;
  test_clist_basics(allocator, tabs + 1);       NEWLINE;
  test_clist_iterators(allocator, tabs + 1);    NEWLINE;
  test_clist_ops(allocator, tabs + 1);          NEWLINE;
}