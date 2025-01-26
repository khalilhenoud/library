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
#include <library/core/core.h>
#include <library/containers/clist.h>
#include <stdint.h>
#include <cassert>
#include <common.h>


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
  ", elem_size: " << list.elem_data.size <<
  ", allocator: " << (uint64_t)(list.allocator) <<
  ", elem_cleanup: " << (uint64_t)(elem_data_get_cleanup_fn(&list.elem_data)) << 
  ", nodes: " << (uint64_t)(list.nodes) << ")" << 
  std::endl;
}

void
test_clist_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  clist_t list; clist_def(&list);
  print_meta(list, tabs);
  CTABS << PRINT_BOOL(clist_is_def(&list)) << std::endl;
}

void
test_clist_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("defines and populate basic types of lists");

  clist_t list_i8; clist_def(&list_i8);
  clist_setup(&list_i8, get_type_data(char), allocator);
  print_meta(list_i8, tabs);
  for (int32_t i = 100; i < 120; ++i)
    clist_push_back(&list_i8, (char)i + 1, char);
  print_meta(list_i8, tabs);
  print_clist_content<char>(list_i8, tabs);
  clist_cleanup(&list_i8, NULL);
  NEWLINE;

  clist_t list_i16; clist_def(&list_i16);
  clist_setup(&list_i16, get_type_data(short), allocator);
  print_meta(list_i16, tabs);
  for (int32_t i = 120; i < 140; ++i)
    clist_push_back(&list_i16, (short)i + 1, short);
  print_meta(list_i16, tabs);
  print_clist_content<short>(list_i16, tabs);
  clist_cleanup(&list_i16, NULL);
  NEWLINE;

  clist_t list_i32; clist_def(&list_i32);
  clist_setup(&list_i32, get_type_data(int32_t), allocator);
  print_meta(list_i32, tabs);
  for (int32_t i = 0; i < 10; ++i)
    clist_push_back(&list_i32, (int32_t)i + 1, int32_t);
  print_meta(list_i32, tabs);
  print_clist_content<int32_t>(list_i32, tabs);
  clist_cleanup(&list_i32, NULL);
  NEWLINE;

  clist_t list_f32; clist_def(&list_f32);
  clist_setup(&list_f32, get_type_data(float), allocator);
  print_meta(list_f32, tabs);
  for (int32_t i = 1; i < 20; ++i)
    clist_push_back(&list_f32, (float)i + 1.75f * i, float);
  print_meta(list_f32, tabs);
  print_clist_content<float>(list_f32, tabs);
  clist_cleanup(&list_f32, NULL);
  NEWLINE;

  clist_t list_f64; clist_def(&list_f64);
  clist_setup(&list_f64, get_type_data(double), allocator);
  print_meta(list_f64, tabs);
  for (int32_t i = 20; i < 35; ++i)
    clist_push_back(&list_f64, (double)i + 1.75 * i, double);
  print_meta(list_f64, tabs);
  print_clist_content<double>(list_f64, tabs);
  clist_cleanup(&list_f64, NULL);
}

void
test_clist_iterators(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("tests for iterators, front, back, push_back functions...");

  clist_t list; clist_def(&list);
  clist_setup(&list, get_type_data(double), allocator);
  print_meta(list, tabs);
  for (int32_t i = 0; i < 5; ++i)
    clist_push_back(&list, (double)i + 1.75 * i, double);

  CTABS << "values: ";
  for (
    clist_iterator_t iter = clist_begin(&list); 
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

  CTABS << "clist_push_front: " << std::endl;
  for (int32_t i = 0; i < 10; ++i)
    clist_push_front(&list, (double)i, double);
  print_clist_content<double>(list, tabs);

  CTABS << "clist_pop_front() loop: " << std::endl;
  while (!clist_empty(&list)) {
    print_clist_content<double>(list, tabs);
    clist_pop_front(&list);
  }

  clist_cleanup(&list, NULL);
}

void
test_clist_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("replicate and fullswap function testing");

  {
    CTABS << "replicate testing: " << std::endl;
    // replicate
    clist_t left; clist_def(&left);
    clist_setup(&left, get_type_data(int32_t), allocator);
    for (int32_t i = 0; i < 10; ++i)
      clist_push_back(&left, i + 1, int32_t);
    print_meta(left, tabs);
    print_clist_content<int32_t>(left, tabs);

    clist_t right; clist_def(&right);
    clist_replicate(&left, &right, allocator);
    print_clist_content<int32_t>(right, tabs);
    clist_cleanup(&right, NULL);

    clist_setup(&right, get_type_data(int32_t), allocator);
    clist_replicate(&left, &right, NULL);
    print_clist_content<int32_t>(right, tabs);
    print_meta(right, tabs);
    
    clist_cleanup(&right, NULL);
    clist_cleanup(&left, NULL);
  }

  {
    CTABS << "fullswap testing: " << std::endl;
    // fullswap
    clist_t left; clist_def(&left);
    clist_setup(&left, get_type_data(int32_t), allocator);
    for (int32_t i = 10; i < 15; ++i)
      clist_push_back(&left, i + 1, int32_t);

    clist_t right; clist_def(&right);
    clist_setup(&right, get_type_data(int32_t), allocator);
    for (int32_t i = 120; i < 130; ++i)
      clist_push_back(&right, i + 1, int32_t);
    
    print_clist_content<int32_t>(left, tabs);
    print_clist_content<int32_t>(right, tabs);

    clist_fullswap(&left, &right);

    print_clist_content<int32_t>(left, tabs);
    print_clist_content<int32_t>(right, tabs);

    clist_cleanup(&right, NULL);
    clist_cleanup(&left, NULL);
  }
}

void
test_clist_mem(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("erase, clear testing...");

  clist_t list; clist_def(&list);
  clist_setup(&list, get_type_data(int32_t), allocator);
  for (auto i = 0; i < 20; ++i)
    clist_push_back(&list, i, int32_t);
  print_clist_content<int32_t>(list, tabs);
  print_meta(list, tabs);
  {
    // erase every other element.
    for (int64_t i = list.size - 1; i >= 0; --i) {
      if (!((i + 1) % 2))
        clist_erase(&list, (size_t)i);
    }

    print_meta(list, tabs);
    print_clist_content<int32_t>(list, tabs);
    NEWLINE;
  }
  {
    clist_clear(&list);
    print_meta(list, tabs);
    for (int32_t i = 0; i < 20; ++i)
      clist_insert(&list, 0, i, int32_t);
    
    print_meta(list, tabs);
    print_clist_content<int32_t>(list, tabs);
  }
  clist_cleanup(&list, NULL); 
}

typedef
struct list_custom_t {
  int32_t* data;
  int32_t count;
  int32_t tabs;
} list_custom_t;

static
void 
elem_cleanup(void *elem_ptr, const allocator_t* allocator)
{
  list_custom_t* casted = (list_custom_t*)elem_ptr;
  if (casted->count && casted->data) {
    std::cout << std::string(casted->tabs, '\t') << "freeing " << 
    casted->count << " elements" << std::endl;
    allocator->mem_free(casted->data);
  } else
    std::cout << std::string(casted->tabs, '\t') << "nothing to free" << 
    std::endl;
}

void
test_clist_custom(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  list_custom_t data;
  memset(&data, 0, sizeof(list_custom_t));
  clist_t list; clist_def(&list);
  clist_setup(&list, get_type_data(list_custom_t), allocator);
  print_meta(list, tabs);
  clist_push_back(&list, data, list_custom_t);
  clist_push_back(&list, data, list_custom_t);
  clist_push_back(&list, data, list_custom_t);

  auto* ptr = clist_as(&list, 0, list_custom_t);
  ptr->count = 3;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  ptr = clist_as(&list, 1, list_custom_t);
  ptr->tabs = tabs;

  ptr = clist_as(&list, 2, list_custom_t);
  ptr->count = 5;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  clist_cleanup(&list, NULL);
}

INITIALIZER(register_list_custom)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = NULL;
  vtable.fn_is_def = NULL;
  vtable.fn_is_equal = NULL;
  vtable.fn_serialize = NULL;
  vtable.fn_deserialize = NULL;
  vtable.fn_cleanup = elem_cleanup;
  register_type(get_type_id(list_custom_t), &vtable);
}

void
test_clist_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_clist_def(allocator, tabs + 1);          NEWLINE;
  test_clist_basics(allocator, tabs + 1);       NEWLINE;
  test_clist_iterators(allocator, tabs + 1);    NEWLINE;
  test_clist_ops(allocator, tabs + 1);          NEWLINE;
  test_clist_mem(allocator, tabs + 1);          NEWLINE;
  test_clist_custom(allocator, tabs + 1);       NEWLINE;
}