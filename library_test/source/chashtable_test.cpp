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
  const static constexpr uint32_t per_row_key_value = 5;
  const static constexpr uint32_t per_row_indices = 10;
  CTABS << "[key, value]: " << std::endl;
  CTABS;
  for (size_t i = 0, j = 1; i < map.values.size; ++i, ++j) {
    std::cout << '[' << *cvector_as(&map.keys, i, KEY_TYPE) << ", "
      << *cvector_as(&map.values, i, VALUE_TYPE) << "] ";
    if (!(j % per_row_key_value) && j != map.values.size) {
      std::cout << std::endl;
      CTABS;
    }
  }
  NEWLINE;
  CTABS << "[indices]: " << std::endl;
  CTABS;
  for (size_t i = 0, j = 1; i < map.indices.size; ++i, ++j) {
    uint64_t index = *cvector_as(&map.indices, i, uint32_t);
    if (index == (uint32_t)-1)
      std::cout << "- ";
    else
      std::cout << index << " ";
    if (!(j % per_row_indices) && j != map.indices.size) {
      std::cout << std::endl;
      CTABS;
    }
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
  "map(max_load_factor: " << map.max_load_factor << ", "<< std::endl;
  CTABS << 
  "  allocator: " << std::hex << (uint64_t)(map.allocator) <<
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

uint64_t 
hash_calc(const void* key)
{
  return *((uint64_t*)key);
}

int32_t 
key_equal(const void* key1, const void* key2)
{
  return hash_calc(key1) == hash_calc(key2);
}

uint64_t 
hash_calc2(const void* key)
{
  const char** str = (const char**)key;
  return hash_fnv1a_64(*str, strlen(*str));
}

int32_t 
key_equal2(const void* key1, const void* key2)
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

void 
cleanup_str(void *elem_ptr, const allocator_t* allocator)
{
  char** ptr = (char**)elem_ptr;
  allocator->mem_free(*ptr);
}

#define DECLARE_HELPER_CHASHTABLE(alias, type_key, type_value)  \
inline void alias##_insert(chashtable_t* map, type_key key, type_value value) \
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
    NEWLINE;
    print_chashtable_content<uint64_t, float>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    NEWLINE;
    for (uint64_t i = 0; i < 100; ++i)
      mullf_insert(&map, i, i * 1.4f);
    print_chashtable_content<uint64_t, float>(map, tabs + 1);
    NEWLINE;
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
    NEWLINE;
    mstri_insert(&map, "khalil", 15);
    mstri_insert(&map, "khalil", 62);
    mstri_insert(&map, "aline", 71);
    mstri_insert(&map, "simone", 80);
    mstri_insert(&map, "simone", 18);
    mstri_insert(&map, "naji", 32);
    mstri_insert(&map, "ibtissam", 167);
    mstri_insert(&map, "mohammad", 22112);
    mstri_insert(&map, "kafa", 809);
    mstri_insert(&map, "hailey", 143);
    mstri_insert(&map, "panda", 11);
    mstri_insert(&map, "echo", 9);
    mstri_insert(&map, "ginny", 1);
    mstri_insert(&map, "ginny", 762);
    mstri_insert(&map, "tarek", 20);
    mstri_insert(&map, "mostafa", 9);
    print_chashtable_content<char*, int32_t>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    chashtable_cleanup(&map);
    NEWLINE;
  }
}

void
test_chashtable_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("replicate and fullswap function testing");

  {
    CTABS << "replicate testing: " << std::endl;
    chashtable_t left = chashtable_def();
    chashtable_setup(
      &left, 
      sizeof(char*), sizeof(int32_t), 
      allocator, 0.6f, 
      cleanup_str, replicate_str, key_equal2, NULL, hash_calc2);
    mstri_insert(&left, "khalil", 15);
    mstri_insert(&left, "khalil", 62);
    mstri_insert(&left, "aline", 71);
    mstri_insert(&left, "simone", 80);
    mstri_insert(&left, "simone", 18);
    mstri_insert(&left, "naji", 32);
    mstri_insert(&left, "ibtissam", 167);
    mstri_insert(&left, "mohammad", 22112);
    mstri_insert(&left, "kafa", 809);
    mstri_insert(&left, "hailey", 143);
    mstri_insert(&left, "panda", 11);
    mstri_insert(&left, "echo", 9);
    mstri_insert(&left, "ginny", 1);
    mstri_insert(&left, "ginny", 762);
    mstri_insert(&left, "tarek", 20);
    mstri_insert(&left, "mostafa", 9);
    print_meta(left, tabs);
    NEWLINE;
    print_chashtable_content<char*, int32_t>(left, tabs + 1);

    chashtable_t right = chashtable_def();
    chashtable_replicate(&left, &right, allocator, NULL);
    NEWLINE;
    print_meta(right, tabs);
    NEWLINE;
    print_chashtable_content<char*, int32_t>(right, tabs + 1);
    chashtable_cleanup(&right);

    chashtable_setup(
      &right,
      sizeof(char*), sizeof(int32_t),
      allocator, 0.6f,
      cleanup_str, replicate_str, key_equal2, NULL, hash_calc2);
    chashtable_replicate(&left, &right, NULL, NULL);
    NEWLINE;
    print_meta(right, tabs);
    NEWLINE;
    print_chashtable_content<char*, int32_t>(right, tabs + 1);
        
    chashtable_cleanup(&right);
    chashtable_cleanup(&left);
  }
  
  {
    CTABS << "fullswap testing: " << std::endl;
    chashtable_t left = chashtable_def();
    chashtable_setup(
      &left, 
      sizeof(char*), sizeof(int32_t), 
      allocator, 0.6f, 
      cleanup_str, replicate_str, key_equal2, NULL, hash_calc2);
    mstri_insert(&left, "khalil", 1);
    mstri_insert(&left, "aline", 2);
    mstri_insert(&left, "simone", 3);
    mstri_insert(&left, "naji", 4);
    mstri_insert(&left, "ibtissam", 5);

    chashtable_t right = chashtable_def();
    chashtable_setup(
      &right, 
      sizeof(char*), sizeof(int32_t), 
      allocator, 0.6f, 
      cleanup_str, replicate_str, key_equal2, NULL, hash_calc2);
    mstri_insert(&right, "elias", 10);
    mstri_insert(&right, "tawas", 20);
    mstri_insert(&right, "charbel", 30);
    mstri_insert(&right, "tony", 40);
    mstri_insert(&right, "karim", 50);

    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    print_chashtable_content<char*, int32_t>(right, tabs + 1);

    chashtable_fullswap(&left, &right);

    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    print_chashtable_content<char*, int32_t>(right, tabs + 1);

    chashtable_cleanup(&right);
    chashtable_cleanup(&left);
  }
}

void
test_chashtable_misc(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("misc function testing");

  {
    chashtable_t left = chashtable_def();
    chashtable_setup(
      &left, 
      sizeof(char*), sizeof(int32_t), 
      allocator, 0.6f, 
      cleanup_str, replicate_str, key_equal2, NULL, hash_calc2);
    mstri_insert(&left, "khalil", 15);
    mstri_insert(&left, "aline", 71);
    mstri_insert(&left, "simone", 80);
    CTABS << chashtable_empty(&left) << std::endl;
    CTABS << chashtable_size(&left) << std::endl;
    CTABS << chashtable_capacity(&left) << std::endl;
    CTABS << chashtable_load_factor(&left) << std::endl;
    CTABS << chashtable_max_load_factor(&left) << std::endl;
    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    chashtable_clear(&left);
    CTABS << chashtable_empty(&left) << std::endl;
    mstri_insert(&left, "naji", 15);
    mstri_insert(&left, "ibtissam", 71);
    mstri_insert(&left, "elie", 80);
    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    {
      int32_t* ptr;
      chashtable_at(&left, "naji", char*, int32_t, ptr);
      CTABS << *ptr << std::endl;
    }
    mstri_insert(&left, "nissan", 90);
    chashtable_rehash(&left, 16);
    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    chashtable_erase(&left, "nissan", char*);
    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    mstri_insert(&left, "nissan", 90);
    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    chashtable_erase(&left, "nissan", char*);
    print_chashtable_content<char*, int32_t>(left, tabs + 1);
    chashtable_cleanup(&left);
    NEWLINE;
  }
}

void
test_chashtable_iterators(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("tests for iterators, begin, end, advance, equal, key, value...");

  chashtable_t map = chashtable_def();
  chashtable_setup(
    &map, 
    sizeof(char*), sizeof(int32_t), 
    allocator, 0.6f, 
    cleanup_str, replicate_str, key_equal2, NULL, hash_calc2);
  mstri_insert(&map, "elias", 10);
  mstri_insert(&map, "tawas", 20);
  mstri_insert(&map, "charbel", 30);
  mstri_insert(&map, "tony", 40);
  mstri_insert(&map, "karim", 50);

  CTABS << "[key, value]: " << std::endl;
  {
    CTABS;
    for (
      chashtable_iterator_t iter = chashtable_begin(&map); 
      !chashtable_iter_equal(iter, chashtable_end(&map)); 
      chashtable_advance(&iter)) {
        std::cout << "[" << *chashtable_key(&iter, char*) << ", "; 
        std::cout << *chashtable_value(&iter, int32_t) << "] ";
      }
    NEWLINE;
  }
  chashtable_cleanup(&map);
}

void
test_chashtable_mem(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("rehash, reserve testing...");

  chashtable_t map = chashtable_def();
  chashtable_setup(
    &map, 
    sizeof(uint64_t), sizeof(float), 
    allocator, 0.6f, 
    NULL, NULL, key_equal, NULL, hash_calc);
  print_meta(map, tabs);
  mullf_insert(&map, 16, 1.12f);
  print_chashtable_content<uint64_t, float>(map, tabs + 1);
  NEWLINE;
  chashtable_reserve(&map, 100 / chashtable_max_load_factor(&map));
  print_meta(map, tabs);
  print_chashtable_content<uint64_t, float>(map, tabs + 1);
  NEWLINE;
  for (uint64_t i = 0; i < 100; ++i)
    mullf_insert(&map, i, i * 1.4f);
  print_meta(map, tabs);
  print_chashtable_content<uint64_t, float>(map, tabs + 1);
  NEWLINE;
  for (uint64_t i = 0; i < 100; ++i)
    chashtable_erase(&map, i, uint64_t);
  print_meta(map, tabs);
  print_chashtable_content<uint64_t, float>(map, tabs + 1);
  chashtable_rehash(&map, 0);
  print_meta(map, tabs);
  print_chashtable_content<uint64_t, float>(map, tabs + 1);
  chashtable_reserve(&map, 0);
  print_meta(map, tabs);
  print_chashtable_content<uint64_t, float>(map, tabs + 1);
  chashtable_cleanup(&map);
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

static
void
test_chashtable_custom(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  custom_t data;
  memset(&data, 0, sizeof(custom_t));
  chashtable_t map = chashtable_def();
  chashtable_setup(
    &map, 
    sizeof(uint64_t), sizeof(custom_t), 
    allocator, 0.6f, 
    NULL, NULL, key_equal, elem_cleanup, hash_calc);
  print_meta(map, tabs);
  chashtable_insert(&map, 16ull, uint64_t, data, custom_t);
  chashtable_insert(&map, 17ull, uint64_t, data, custom_t);
  chashtable_insert(&map, 32ull, uint64_t, data, custom_t);

  custom_t* ptr; chashtable_at(&map, 16ull, uint64_t, custom_t, ptr);
  ptr->count = 3;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  chashtable_at(&map, 17ull, uint64_t, custom_t, ptr);
  ptr->tabs = tabs;

  chashtable_at(&map, 32ull, uint64_t, custom_t, ptr);
  ptr->count = 5;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  chashtable_cleanup(&map);
}

void
test_chashtable_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_chashtable_def(allocator, tabs + 1);          NEWLINE;
  test_chashtable_basics(allocator, tabs + 1);       NEWLINE;
  test_chashtable_ops(allocator, tabs + 1);          NEWLINE;
  test_chashtable_misc(allocator, tabs + 1);         NEWLINE;
  test_chashtable_mem(allocator, tabs + 1);          NEWLINE;
  test_chashtable_iterators(allocator, tabs + 1);    NEWLINE;
  test_chashtable_custom(allocator, tabs + 1);       NEWLINE;
}