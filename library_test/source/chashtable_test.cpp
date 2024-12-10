/**
 * @file chashtable_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief tests for hashtable functionality
 * @version 0.1
 * @date 2024-11-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <library/allocator/allocator.h>
#include <library/containers/chashtable.h>
#include <library/hash/fnv.h>
#include <stdint.h>
#include <cassert>
#include <common.h>


template<typename KEY_TYPE, typename VALUE_TYPE>
void
print_chashtable_content(chashtable_t& map, const int32_t tabs)
{
  CTABS << "[key, value]: " << std::endl;
  CTABS;
  for (size_t i = 0; i < map.values.size; ++i)
    std::cout << '[' << *cvector_as(&map.keys, i, KEY_TYPE) << ", "
      << *cvector_as(&map.values, i, VALUE_TYPE) << "] ";
  NEWLINE;
  CTABS << "indices: ";
  for (size_t i = 0, j = 0; i < map.indices.size; ++i, ++j) {
    uint64_t index = *cvector_as(&map.indices, i, uint32_t);
    if (index == (uint32_t)-1)
      std::cout << "- ";
    else
      std::cout << index << " ";
  }
  NEWLINE;
}

static
void
print_meta(chashtable_t& map, const int32_t tabs) 
{
  std::cout << std::dec;
  CTABS << 
  "map(values[size: " << map.values.size << 
  ", elem_size: " << map.values.elem_size << 
  ", capacity: " << map.values.capacity << 
  ", allocator: " << std::dec << (uint64_t)(map.allocator) <<
  ", elem_cleanup: " << std::dec << (uint64_t)(map.values.elem_cleanup) 
  << "])" << std::endl;
  std::cout << std::dec;
  CTABS << 
  "map(keys[size: " << map.keys.size << 
  ", elem_size: " << map.keys.elem_size << 
  ", capacity: " << map.keys.capacity << 
  ", allocator: " << std::hex << (uint64_t)(map.allocator) <<
  ", elem_cleanup: " << std::hex << (uint64_t)(map.keys.elem_cleanup) << 
  "])" << std::endl;
  std::cout << std::dec;
  CTABS << 
  "map(indices[size: " << map.indices.size << 
  ", elem_size: " << map.indices.elem_size << 
  ", capacity: " << map.indices.capacity << 
  ", allocator: " << std::hex << (uint64_t)(map.allocator) <<
  ", elem_cleanup: " << std::hex << (uint64_t)(map.indices.elem_cleanup) 
  << "])" << std::endl;
  std::cout << std::dec;
  CTABS << 
  "map(max_load_factor: " << map.max_load_factor << 
  ", allocator: " << std::hex << (uint64_t)(map.allocator) <<
  ", key_replicate: " << std::hex << (uint64_t)(map.key_replicate) << 
  ", key_equal: " << std::hex << (uint64_t)(map.key_equal) << 
  ", hash_calc: " << std::hex << (uint64_t)(map.hash_calc) << ")" << std::endl; 
  std::cout << std::dec;
}

void
test_chashtable_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  chashtable_t map = chashtable_def();
  print_meta(map, tabs);
  CTABS << PRINT_BOOL(chashtable_is_def(&map)) << std::endl;
}

uint64_t hash_calc(const void* key)
{
  return *((uint64_t*)key);
}

int32_t key_equal(const void* key1, const void* key2)
{
  return hash_calc(key1) == hash_calc(key2);
}

uint64_t hash_calc2(const void* key)
{
  const char** str = (const char**)key;
  return hash_fnv1a_64(*str, strlen(*str));
}

int32_t key_equal2(const void* key1, const void* key2)
{
  const char** str_key1 = (const char**)key1;
  const char** str_key2 = (const char**)key2;
  return strcmp(*str_key1, *str_key2) == 0;
}

void 
replicate_str(const void* src, void* dst, const allocator_t* allocator)
{
  const char** source_str = (const char**)src;
  char** dst_str = (char**)dst;
  *dst_str = (char*)allocator->mem_alloc(strlen(*source_str) + 1);
  memcpy(*dst_str, *source_str, strlen(*source_str) + 1);
}

void cleanup_str(void *elem_ptr, const allocator_t* allocator)
{
  char** ptr = (char**)elem_ptr;
  allocator->mem_free(*ptr);
}

#define DECLARE_HELPER_CHASHTABLE(alias, type_key, type_value)  \
void alias##_insert(chashtable_t* map, type_key key, type_value value) \
{ \
 chashtable_insert(map, key, type_key, value, type_value); \
}

DECLARE_HELPER_CHASHTABLE(mullf, uint64_t, float)
DECLARE_HELPER_CHASHTABLE(mstri, char*, int32_t)

void
test_chashtable_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("defines and populate basic types of hashtables");

  {
    chashtable_t map = chashtable_def();
    chashtable_setup(
      &map, 
      sizeof(uint64_t), sizeof(float), 
      allocator, 0.6f, 
      NULL, NULL, key_equal, NULL, hash_calc);
    print_meta(map, tabs);
    mullf_insert(&map, 16, 1.12f);
    print_chashtable_content<uint64_t, float>(map, tabs + 1);
    print_meta(map, tabs);
    for (uint64_t i = 0; i < 100; ++i)
      mullf_insert(&map, i, i * 1.4f);
    print_chashtable_content<uint64_t, float>(map, tabs + 1);
    print_meta(map, tabs);
    chashtable_cleanup(&map);
    NEWLINE;
  }

  {
    // NOTE: this case brings up the need to replicate the key and or value upon
    // insertion. something like copy key, copy value.
    chashtable_t map = chashtable_def();
    chashtable_setup(
      &map, 
      sizeof(char*), sizeof(int32_t), 
      allocator, 0.6f, 
      cleanup_str, replicate_str, key_equal2, NULL, hash_calc2);
    print_meta(map, tabs);
    chashtable_insert(&map, "khalil", char*, 5, int32_t);
    chashtable_insert(&map, "khalil", char*, 6, int32_t);
    chashtable_insert(&map, "aline", char*, 7, int32_t);
    chashtable_insert(&map, "simone", char*, 8, int32_t);
    

    print_chashtable_content<char*, int32_t>(map, tabs + 1);
    // print_meta(map, tabs);
    // for (uint64_t i = 0; i < 100; ++i)
    //   mullf_insert(&map, i, i * 1.4f);
    // print_chashtable_content<uint64_t, float>(map, tabs + 1);
    print_meta(map, tabs);
    chashtable_cleanup(&map);
    NEWLINE;
  }


  // cvector_t vec_i16 = cvector_def();
  // cvector_setup(&vec_i16, sizeof(short), 4, allocator, NULL);
  // print_meta(vec_i16, tabs);
  // for (int32_t i = 120; i < 140; ++i)
  //   cvector_push_back(&vec_i16, (short)i + 1, short);
  // print_meta(vec_i16, tabs);
  // print_cvector_content<short>(vec_i16, tabs);
  // cvector_cleanup(&vec_i16);
  // NEWLINE;

  // cvector_t vec_i32 = cvector_def();
  // cvector_setup(&vec_i32, sizeof(int32_t), 4, allocator, NULL);
  // print_meta(vec_i32, tabs);
  // for (int32_t i = 0; i < 10; ++i)
  //   cvector_push_back(&vec_i32, (int32_t)i + 1, int32_t);
  // print_meta(vec_i32, tabs);
  // print_cvector_content<int32_t>(vec_i32, tabs);
  // cvector_cleanup(&vec_i32);
  // NEWLINE;

  // cvector_t vec_f32 = cvector_def();
  // cvector_setup(&vec_f32, sizeof(float), 4, allocator, NULL);
  // print_meta(vec_f32, tabs);
  // for (int32_t i = 1; i < 20; ++i)
  //   cvector_push_back(&vec_f32, (float)i + 1.75f * i, float);
  // print_meta(vec_f32, tabs);
  // print_cvector_content<float>(vec_f32, tabs);
  // cvector_cleanup(&vec_f32);
  // NEWLINE;

  // cvector_t vec_f64 = cvector_def();
  // cvector_setup(&vec_f64, sizeof(double), 4, allocator, NULL);
  // print_meta(vec_f64, tabs);
  // for (int32_t i = 20; i < 35; ++i)
  //   cvector_push_back(&vec_f64, (double)i + 1.75 * i, double);
  // print_meta(vec_f64, tabs);
  // print_cvector_content<double>(vec_f64, tabs);
  // cvector_cleanup(&vec_f64);
}

/*
void
test_cvector_iterators(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("tests for iterators, front, back, push_back functions...");

  cvector_t vec = cvector_def();
  cvector_setup(&vec, sizeof(double), 16, allocator, NULL);
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
  
  cvector_cleanup(&vec);
}

void
test_cvector_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("replicate and fullswap function testing");

  {
    CTABS << "replicate testing: " << std::endl;
    // replicate
    cvector_t left = cvector_def();
    cvector_setup(&left, sizeof(int32_t), 16, allocator, NULL);
    for (int32_t i = 0; i < 10; ++i)
      cvector_push_back(&left, i + 1, int32_t);
    print_meta(left, tabs);
    print_cvector_content<int32_t>(left, tabs);

    cvector_t right = cvector_def();
    cvector_replicate(&left, &right, allocator, NULL);
    print_cvector_content<int32_t>(right, tabs);
    cvector_cleanup(&right);

    cvector_setup(&right, sizeof(int32_t), 32, allocator, NULL);
    cvector_replicate(&left, &right, NULL, NULL);
    print_cvector_content<int32_t>(right, tabs);
    print_meta(right, tabs);
    
    cvector_cleanup(&right);
    cvector_cleanup(&left);
  }
  
  {
    CTABS << "fullswap testing: " << std::endl;
    // fullswap
    cvector_t left = cvector_def();
    cvector_setup(&left, sizeof(int32_t), 16, allocator, NULL);
    for (int32_t i = 10; i < 15; ++i)
      cvector_push_back(&left, i + 1, int32_t);

    cvector_t right = cvector_def();
    cvector_setup(&right, sizeof(int32_t), 20, allocator, NULL);
    for (int32_t i = 120; i < 130; ++i)
      cvector_push_back(&right, i + 1, int32_t);
    
    print_cvector_content<int32_t>(left, tabs);
    print_cvector_content<int32_t>(right, tabs);

    cvector_fullswap(&left, &right);

    print_cvector_content<int32_t>(left, tabs);
    print_cvector_content<int32_t>(right, tabs);

    cvector_cleanup(&right);
    cvector_cleanup(&left);
  }
}

void
test_cvector_mem(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("resize, clear, shrink_to_fit, reserve testing...");

  cvector_t vec = cvector_def();
  cvector_setup(&vec, sizeof(int32_t), 16, allocator, NULL);
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
  cvector_cleanup(&vec); 
}

typedef
struct custom_t {
  int32_t* data;
  int32_t count;
  int32_t tabs;
} custom_t;

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

static
void
test_cvector_custom(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  custom_t data;
  memset(&data, 0, sizeof(custom_t));
  cvector_t vec = cvector_def();
  cvector_setup(&vec, sizeof(custom_t), 4, allocator, elem_cleanup);
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

  cvector_cleanup(&vec);
}
*/

void
test_chashtable_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_chashtable_def(allocator, tabs + 1);          NEWLINE;
  test_chashtable_basics(allocator, tabs + 1);       NEWLINE;
}