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
#include <library/core/core.h>
#include <library/containers/cvector.h>
#include <stdint.h>
#include <cassert>
#include <common.h>


template<typename T>
void
print_cvector_content(cvector_t& vec, const int32_t tabs)
{
  CTABS << "values: ";
  for (size_t i = 0; i < vec.size; ++i)
    std::cout << *cvector_as(&vec, i, T) << " ";
  NEWLINE;
}

static
void
print_meta(cvector_t& vec, const int32_t tabs) 
{
  CTABS << 
  "vec(size: " << vec.size << 
  ", elem_size: " << vec.elem_data.size << 
  ", capacity: " << vec.capacity << 
  ", allocator: " << (uint64_t)(vec.allocator) <<
  ", elem_cleanup: " << (uint64_t)(elem_data_get_cleanup_fn(&vec.elem_data)) << 
  ")" << std::endl;
}

void
test_cvector_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cvector_t vec; cvector_def(&vec);
  print_meta(vec, tabs);
  CTABS << PRINT_BOOL(cvector_is_def(&vec)) << std::endl;
}

void
test_cvector_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("defines and populate basic types of vectors");

  cvector_t vec_i8;
  cvector_def(&vec_i8);
  cvector_setup(&vec_i8, get_type_data(char), 4, allocator);
  print_meta(vec_i8, tabs);
  for (int32_t i = 100; i < 120; ++i)
    cvector_push_back(&vec_i8, (char)i + 1, char);
  print_meta(vec_i8, tabs);
  print_cvector_content<char>(vec_i8, tabs);
  cvector_cleanup(&vec_i8, NULL);
  NEWLINE;

  cvector_t vec_i16;
  cvector_def(&vec_i16);
  cvector_setup(&vec_i16, get_type_data(short), 4, allocator);
  print_meta(vec_i16, tabs);
  for (int32_t i = 120; i < 140; ++i)
    cvector_push_back(&vec_i16, (short)i + 1, short);
  print_meta(vec_i16, tabs);
  print_cvector_content<short>(vec_i16, tabs);
  cvector_cleanup(&vec_i16, NULL);
  NEWLINE;

  cvector_t vec_i32;
  cvector_def(&vec_i32);
  cvector_setup(&vec_i32, get_type_data(int32_t), 4, allocator);
  print_meta(vec_i32, tabs);
  for (int32_t i = 0; i < 10; ++i)
    cvector_push_back(&vec_i32, (int32_t)i + 1, int32_t);
  print_meta(vec_i32, tabs);
  print_cvector_content<int32_t>(vec_i32, tabs);
  cvector_cleanup(&vec_i32, NULL);
  NEWLINE;

  cvector_t vec_f32;
  cvector_def(&vec_f32);
  cvector_setup(&vec_f32, get_type_data(float), 4, allocator);
  print_meta(vec_f32, tabs);
  for (int32_t i = 1; i < 20; ++i)
    cvector_push_back(&vec_f32, (float)i + 1.75f * i, float);
  print_meta(vec_f32, tabs);
  print_cvector_content<float>(vec_f32, tabs);
  cvector_cleanup(&vec_f32, NULL);
  NEWLINE;

  cvector_t vec_f64;
  cvector_def(&vec_f64);
  cvector_setup(&vec_f64, get_type_data(double), 4, allocator);
  print_meta(vec_f64, tabs);
  for (int32_t i = 20; i < 35; ++i)
    cvector_push_back(&vec_f64, (double)i + 1.75 * i, double);
  print_meta(vec_f64, tabs);
  print_cvector_content<double>(vec_f64, tabs);
  cvector_cleanup(&vec_f64, NULL);
}

void
test_cvector_iterators(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("tests for iterators, front, back, push_back functions...");

  cvector_t vec;
  cvector_def(&vec);
  cvector_setup(&vec, get_type_data(double), 16, allocator);
  print_meta(vec, tabs);
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
  CTABS << "back: " << *cvector_back(&vec, double) << std::endl;

  CTABS << "cvector_shrink_to_fit(&vec)" << std::endl;
  cvector_shrink_to_fit(&vec);
  print_meta(vec, tabs);

  CTABS << "cvector_pop_back() loop: " << std::endl;
  while (!cvector_empty(&vec)) {
    print_cvector_content<double>(vec, tabs);
    cvector_pop_back(&vec);
  }
  
  cvector_cleanup(&vec, NULL);
}

void
test_cvector_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("replicate and fullswap function testing");

  {
    CTABS << "replicate testing: " << std::endl;
    // replicate
    cvector_t left; cvector_def(&left);
    cvector_setup(&left, get_type_data(int32_t), 16, allocator);
    for (int32_t i = 0; i < 10; ++i)
      cvector_push_back(&left, i + 1, int32_t);
    print_meta(left, tabs);
    print_cvector_content<int32_t>(left, tabs);

    cvector_t right; cvector_def(&right);
    cvector_replicate(&left, &right, allocator);
    print_cvector_content<int32_t>(right, tabs);
    cvector_cleanup(&right, NULL);

    cvector_setup(&right, get_type_data(int32_t), 32, allocator);
    cvector_replicate(&left, &right, NULL);
    print_cvector_content<int32_t>(right, tabs);
    print_meta(right, tabs);
    
    cvector_cleanup(&right, NULL);
    cvector_cleanup(&left, NULL);
  }
  
  {
    CTABS << "fullswap testing: " << std::endl;
    // fullswap
    cvector_t left; cvector_def(&left);
    cvector_setup(&left, get_type_data(int32_t), 16, allocator);
    for (int32_t i = 10; i < 15; ++i)
      cvector_push_back(&left, i + 1, int32_t);

    cvector_t right; cvector_def(&right);
    cvector_setup(&right, get_type_data(int32_t), 20, allocator);
    for (int32_t i = 120; i < 130; ++i)
      cvector_push_back(&right, i + 1, int32_t);
    
    print_cvector_content<int32_t>(left, tabs);
    print_cvector_content<int32_t>(right, tabs);

    cvector_fullswap(&left, &right);

    print_cvector_content<int32_t>(left, tabs);
    print_cvector_content<int32_t>(right, tabs);

    cvector_cleanup(&right, NULL);
    cvector_cleanup(&left, NULL);
  }
}

void
test_cvector_mem(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("resize, clear, shrink_to_fit, reserve testing...");

  cvector_t vec; cvector_def(&vec);
  cvector_setup(&vec, get_type_data(int32_t), 16, allocator);
  cvector_resize(&vec, 20);
  for (auto i = 0; i < vec.size; ++i)
    *(int32_t*)cvector_at(&vec, i) = i;
  print_cvector_content<int32_t>(vec, tabs);
  print_meta(vec, tabs);
  {
    // erase every other element.
    for (int64_t i = vec.size - 1; i >= 0; --i) {
      if (!((i + 1) % 2))
        cvector_erase(&vec, (size_t)i);
    }

    print_meta(vec, tabs);
    print_cvector_content<int32_t>(vec, tabs);
    NEWLINE;
  }
  {
    cvector_clear(&vec);
    cvector_shrink_to_fit(&vec);
    print_meta(vec, tabs);

    for (int32_t i = 0; i < 20; ++i)
      cvector_insert(&vec, 0, i, int32_t);
    print_meta(vec, tabs);
    print_cvector_content<int32_t>(vec, tabs);
    cvector_shrink_to_fit(&vec);
    print_meta(vec, tabs);
    NEWLINE;
  }
  {
    cvector_clear(&vec);
    cvector_shrink_to_fit(&vec);
    print_meta(vec, tabs);
    cvector_reserve(&vec, 20);
    print_meta(vec, tabs);

    for (int32_t i = 0; i < 20; ++i)
      cvector_insert(&vec, 0, i, int32_t);
    print_meta(vec, tabs);
    print_cvector_content<int32_t>(vec, tabs);
    cvector_shrink_to_fit(&vec);
    print_meta(vec, tabs);
  }
  cvector_cleanup(&vec, NULL); 
}

typedef
struct custom_t {
  int32_t* data;
  int32_t count;
  int32_t tabs;
} custom_t;

static
void
elem_cleanup(void *elem_ptr, const allocator_t* allocator)
{
  custom_t* casted = (custom_t*)elem_ptr;
  if (casted->count && casted->data) {
    std::cout << std::string(casted->tabs, '\t') << "freeing " << 
    casted->count << " elements" << std::endl;
    allocator->mem_free(casted->data);
  } else
    std::cout << std::string(casted->tabs, '\t') << "nothing to free" << 
    std::endl;
}

INITIALIZER(register_custom)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = NULL;
  vtable.fn_is_def = NULL;
  vtable.fn_is_equal = NULL;
  vtable.fn_serialize = NULL;
  vtable.fn_deserialize = NULL;
  vtable.fn_cleanup = elem_cleanup;
  register_type(get_type_id(custom_t), &vtable);
}

static
void
test_cvector_custom(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  custom_t data;
  memset(&data, 0, sizeof(custom_t));
  cvector_t vec; cvector_def(&vec);
  cvector_setup(&vec, get_type_data(custom_t), 4, allocator);
  print_meta(vec, tabs);
  cvector_push_back(&vec, data, custom_t);
  cvector_push_back(&vec, data, custom_t);
  cvector_push_back(&vec, data, custom_t);

  auto* ptr = cvector_as(&vec, 0, custom_t);
  ptr->count = 3;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  ptr = cvector_as(&vec, 1, custom_t);
  ptr->tabs = tabs;

  ptr = cvector_as(&vec, 2, custom_t);
  ptr->count = 5;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  cvector_cleanup(&vec, NULL);
}

void
test_cvector_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_cvector_def(allocator, tabs + 1);          NEWLINE;
  test_cvector_basics(allocator, tabs + 1);       NEWLINE;
  test_cvector_iterators(allocator, tabs + 1);    NEWLINE;
  test_cvector_ops(allocator, tabs + 1);          NEWLINE;
  test_cvector_mem(allocator, tabs + 1);          NEWLINE;
  test_cvector_custom(allocator, tabs + 1);       NEWLINE;
}