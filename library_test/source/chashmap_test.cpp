/**
 * @file chashmap_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief tests for hashmap functionality
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <cassert>
#include <cstdint>
#include <string>
#include <classroom.h>
#include <common.h>
#include <library/allocator/allocator.h>
#include <library/containers/chashmap.h>
#include <library/core/core.h>
#include <library/hash/fnv.h>


typedef uint64_t u64;
typedef char* cptr;

template<typename KEY_TYPE, typename VALUE_TYPE>
void
print_chashmap_content(chashmap_t& map, const int32_t tabs)
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
    u64 index = *cvector_as(&map.indices, i, uint32_t);
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
print_meta(chashmap_t& map, const int32_t tabs)
{
  std::cout << std::dec;
  CTABS <<
  "map(values[size: " << map.values.size <<
  ", elem_size: " << map.values.elem_data.size <<
  ", capacity: " << map.values.capacity <<
  ", allocator: " << std::dec << (u64)(map.allocator) <<
  ", elem_cleanup: " << std::dec <<
  (u64)(elem_data_get_cleanup_fn(&map.values.elem_data)) <<
  "])" << std::endl;
  std::cout << std::dec;
  CTABS <<
  "map(keys[size: " << map.keys.size <<
  ", elem_size: " << map.keys.elem_data.size <<
  ", capacity: " << map.keys.capacity <<
  ", allocator: " << std::hex << (u64)(map.allocator) <<
  ", elem_cleanup: " << std::hex <<
  (u64)(elem_data_get_cleanup_fn(&map.keys.elem_data)) <<
  "])" << std::endl;
  std::cout << std::dec;
  CTABS <<
  "map(indices[size: " << map.indices.size <<
  ", elem_size: " << map.indices.elem_data.size <<
  ", capacity: " << map.indices.capacity <<
  ", allocator: " << std::hex << (u64)(map.allocator) <<
  ", elem_cleanup: " << std::hex <<
  (u64)(elem_data_get_cleanup_fn(&map.indices.elem_data)) <<
  "])" << std::endl;
  std::cout << std::dec;
  CTABS <<
  "map(max_load_factor: " << map.max_load_factor << ", "<< std::endl;
  CTABS <<
  "  allocator: " << std::hex << (u64)(map.allocator) <<
  ", key_replicate: " << std::hex << (u64)(chashmap_key_replicate(&map)) <<
  ", key_equal: " << std::hex << (u64)(chashmap_key_equal(&map)) <<
  ", hash_calc: " << std::hex << (u64)(chashmap_hash_calc(&map)) << ")" <<
  std::endl;
  std::cout << std::dec;
}

void
test_chashmap_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  chashmap_t map; chashmap_def(&map);
  print_meta(map, tabs);
  CTABS << PRINT_BOOL(chashmap_is_def(&map)) << std::endl;
}

uint32_t
hash_calc(const void* key)
{
  return *((uint32_t*)key);
}

uint32_t
key_equal(const void* key1, const void* key2)
{
  return *(uint64_t *)(key1) == *(uint64_t *)(key2);
}

uint32_t
hash_calc_str(const void* key)
{
  const cptr* str = (const cptr*)key;
  return hash_fnv1a_32(*str, strlen(*str));
}

uint32_t
key_equal_str(const void* key1, const void* key2)
{
  const cptr* str_key1 = (const cptr*)key1;
  const cptr* str_key2 = (const cptr*)key2;
  return strcmp(*str_key1, *str_key2) == 0;
}

void
replicate_str(const void* src, void* dst, const allocator_t* allocator)
{
  const cptr* source_str = (const cptr*)src;
  cptr* dst_str = (cptr*)dst;
  *dst_str = (cptr)allocator->mem_alloc(strlen(*source_str) + 1);
  memcpy(*dst_str, *source_str, strlen(*source_str) + 1);
}

void
cleanup_str(void *elem_ptr, const allocator_t* allocator)
{
  cptr* ptr = (cptr*)elem_ptr;
  allocator->mem_free(*ptr);
}

#define DECLARE_HELPER_CHASHTABLE(alias, type_key, type_value)              \
inline void alias##_insert(chashmap_t* map, type_key key, type_value value) \
{                                                                           \
 chashmap_insert(map, key, type_key, value, type_value);                    \
}

DECLARE_HELPER_CHASHTABLE(mullf, u64, float)
DECLARE_HELPER_CHASHTABLE(mstri, cptr, int32_t)

void
test_chashmap_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("defines and populate basic types of hashmaps");

  {
    chashmap_t map; chashmap_def(&map);
    chashmap_setup(
      &map,
      get_type_data(u64), get_type_data(float),
      allocator, 0.6f);
    print_meta(map, tabs);
    mullf_insert(&map, 16, 1.12f);
    NEWLINE;
    print_chashmap_content<u64, float>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    NEWLINE;
    for (u64 i = 0; i < 100; ++i)
      mullf_insert(&map, i, i * 1.4f);
    print_chashmap_content<u64, float>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    chashmap_cleanup(&map, NULL);
    NEWLINE;
  }

  {
    // NOTE: this case brings up the need to replicate the key and or value upon
    // insertion. something like copy key, copy value.
    chashmap_t map; chashmap_def(&map);
    chashmap_setup(
      &map,
      get_type_data(cptr), get_type_data(int32_t),
      allocator, 0.6f);
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
    print_chashmap_content<cptr, int32_t>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    chashmap_cleanup(&map, NULL);
    NEWLINE;
  }
}

void
test_chashmap_def_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("defines and populate basic pre-registered types of hashmaps");

  {
    chashmap_t map; chashmap_def(&map);
    chashmap_setup(
      &map,
      get_type_data(uint64_t), get_type_data(float),
      allocator, 0.6f);
    print_meta(map, tabs);
    chashmap_insert(&map, 16, uint64_t, 1.12f, float);
    print_chashmap_content<uint64_t, float>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    NEWLINE;
    for (uint64_t i = 0; i < 20; ++i)
      chashmap_insert(&map, i, uint64_t, (i * 1.4f), float);
    print_chashmap_content<uint64_t, float>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    chashmap_cleanup(&map, NULL);
  }
}

void
test_chashmap_def_basics_with_macros(
  const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("defines and populate basic pre-registered types of hashmaps");

  {
    chashmap_t map;
    chashmap_setup2(&map, uint64_t, float);
    print_meta(map, tabs);
    chashmap_insert(&map, 16, uint64_t, 1.12f, float);
    print_chashmap_content<uint64_t, float>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    NEWLINE;
    for (uint64_t i = 0; i < 20; ++i)
      chashmap_insert(&map, i, uint64_t, (i * 1.4f), float);
    print_chashmap_content<uint64_t, float>(map, tabs + 1);
    NEWLINE;
    print_meta(map, tabs);
    chashmap_cleanup2(&map);
  }
}

void
test_chashmap_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("replicate and fullswap function testing");

  {
    CTABS << "replicate testing: " << std::endl;
    chashmap_t left; chashmap_def(&left);
    chashmap_setup(
      &left,
      get_type_data(cptr), get_type_data(int32_t),
      allocator, 0.6f);
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
    print_chashmap_content<cptr, int32_t>(left, tabs + 1);

    chashmap_t right; chashmap_def(&right);
    chashmap_replicate(&left, &right, allocator);
    NEWLINE;
    print_meta(right, tabs);
    NEWLINE;
    print_chashmap_content<cptr, int32_t>(right, tabs + 1);
    chashmap_cleanup(&right, NULL);

    chashmap_setup(
      &right,
      get_type_data(cptr), get_type_data(int32_t),
      allocator, 0.6f);
    chashmap_replicate(&left, &right, NULL);
    NEWLINE;
    print_meta(right, tabs);
    NEWLINE;
    print_chashmap_content<cptr, int32_t>(right, tabs + 1);

    chashmap_cleanup(&right, NULL);
    chashmap_cleanup(&left, NULL);
  }

  {
    CTABS << "fullswap testing: " << std::endl;
    chashmap_t left; chashmap_def(&left);
    chashmap_setup(
      &left,
      get_type_data(cptr), get_type_data(int32_t),
      allocator, 0.6f);
    mstri_insert(&left, "khalil", 1);
    mstri_insert(&left, "aline", 2);
    mstri_insert(&left, "simone", 3);
    mstri_insert(&left, "naji", 4);
    mstri_insert(&left, "ibtissam", 5);

    chashmap_t right; chashmap_def(&right);
    chashmap_setup(
      &right,
      get_type_data(cptr), get_type_data(int32_t),
      allocator, 0.6f);
    mstri_insert(&right, "elias", 10);
    mstri_insert(&right, "tawas", 20);
    mstri_insert(&right, "charbel", 30);
    mstri_insert(&right, "tony", 40);
    mstri_insert(&right, "karim", 50);

    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    print_chashmap_content<cptr, int32_t>(right, tabs + 1);

    chashmap_fullswap(&left, &right);

    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    print_chashmap_content<cptr, int32_t>(right, tabs + 1);

    chashmap_cleanup(&right, NULL);
    chashmap_cleanup(&left, NULL);
  }
}

void
test_chashmap_misc(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("misc function testing");

  {
    chashmap_t left; chashmap_def(&left);
    chashmap_setup(
      &left,
      get_type_data(cptr), get_type_data(int32_t),
      allocator, 0.6f);
    mstri_insert(&left, "khalil", 15);
    mstri_insert(&left, "aline", 71);
    mstri_insert(&left, "simone", 80);
    CTABS << chashmap_empty(&left) << std::endl;
    CTABS << chashmap_size(&left) << std::endl;
    CTABS << chashmap_capacity(&left) << std::endl;
    CTABS << chashmap_load_factor(&left) << std::endl;
    CTABS << chashmap_max_load_factor(&left) << std::endl;
    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    chashmap_clear(&left);
    CTABS << chashmap_empty(&left) << std::endl;
    mstri_insert(&left, "naji", 15);
    mstri_insert(&left, "ibtissam", 71);
    mstri_insert(&left, "elie", 80);
    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    {
      int32_t* ptr;
      chashmap_at(&left, "naji", cptr, int32_t, ptr);
      CTABS << *ptr << std::endl;
    }
    mstri_insert(&left, "nissan", 90);
    chashmap_rehash(&left, 16);
    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    chashmap_erase(&left, "nissan", cptr);
    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    mstri_insert(&left, "nissan", 90);
    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    chashmap_erase(&left, "nissan", cptr);
    print_chashmap_content<cptr, int32_t>(left, tabs + 1);
    chashmap_cleanup(&left, NULL);
    NEWLINE;
  }
}

void
test_chashmap_iterators(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("tests for iterators, begin, end, advance, equal, key, value...");

  chashmap_t map; chashmap_def(&map);
  chashmap_setup(
    &map,
    get_type_data(cptr), get_type_data(int32_t),
    allocator, 0.6f);
  mstri_insert(&map, "elias", 10);
  mstri_insert(&map, "tawas", 20);
  mstri_insert(&map, "charbel", 30);
  mstri_insert(&map, "tony", 40);
  mstri_insert(&map, "karim", 50);

  CTABS << "[key, value]: " << std::endl;
  {
    CTABS;
    for (
      chashmap_iterator_t iter = chashmap_begin(&map);
      !chashmap_iter_equal(iter, chashmap_end(&map));
      chashmap_advance(&iter)) {
        std::cout << "[" << *chashmap_key(&iter, cptr) << ", ";
        std::cout << *chashmap_value(&iter, int32_t) << "] ";
      }
    NEWLINE;
  }
  chashmap_cleanup(&map, NULL);
}

void
test_chashmap_mem(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("rehash, reserve testing...");

  chashmap_t map; chashmap_def(&map);
  chashmap_setup(
    &map,
    get_type_data(u64), get_type_data(float),
    allocator, 0.6f);
  print_meta(map, tabs);
  mullf_insert(&map, 16, 1.12f);
  print_chashmap_content<u64, float>(map, tabs + 1);
  NEWLINE;
  chashmap_reserve(&map, 100 / chashmap_max_load_factor(&map));
  print_meta(map, tabs);
  print_chashmap_content<u64, float>(map, tabs + 1);
  NEWLINE;
  for (u64 i = 0; i < 100; ++i)
    mullf_insert(&map, i, i * 1.4f);
  print_meta(map, tabs);
  print_chashmap_content<u64, float>(map, tabs + 1);
  NEWLINE;
  for (u64 i = 0; i < 100; ++i)
    chashmap_erase(&map, i, u64);
  print_meta(map, tabs);
  print_chashmap_content<u64, float>(map, tabs + 1);
  chashmap_rehash(&map, 0);
  print_meta(map, tabs);
  print_chashmap_content<u64, float>(map, tabs + 1);
  chashmap_reserve(&map, 0);
  print_meta(map, tabs);
  print_chashmap_content<u64, float>(map, tabs + 1);
  chashmap_cleanup(&map, NULL);
}

typedef
struct hashmap_custom_t {
  int32_t* data;
  int32_t count;
  int32_t tabs;
} hashmap_custom_t;

static
void
elem_cleanup(void *elem_ptr, const allocator_t* allocator)
{
  hashmap_custom_t* casted = (hashmap_custom_t*)elem_ptr;
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
test_chashmap_custom(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  hashmap_custom_t data;
  memset(&data, 0, sizeof(hashmap_custom_t));
  chashmap_t map; chashmap_def(&map);
  chashmap_setup(
    &map,
    get_type_data(u64), get_type_data(hashmap_custom_t),
    allocator, 0.6f);
  print_meta(map, tabs);
  chashmap_insert(&map, 16ull, u64, data, hashmap_custom_t);
  chashmap_insert(&map, 17ull, u64, data, hashmap_custom_t);
  chashmap_insert(&map, 32ull, u64, data, hashmap_custom_t);

  hashmap_custom_t* ptr; chashmap_at(&map, 16ull, u64, hashmap_custom_t, ptr);
  ptr->count = 3;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  chashmap_at(&map, 17ull, u64, hashmap_custom_t, ptr);
  ptr->tabs = tabs;

  chashmap_at(&map, 32ull, u64, hashmap_custom_t, ptr);
  ptr->count = 5;
  ptr->data = (int32_t*)allocator->mem_cont_alloc(ptr->count, sizeof(int32_t));
  ptr->tabs = tabs;

  chashmap_cleanup(&map, NULL);
}

INITIALIZER(register_hashmap_custom)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = NULL;
  vtable.fn_is_def = NULL;
  vtable.fn_is_equal = NULL;
  vtable.fn_serialize = NULL;
  vtable.fn_deserialize = NULL;
  vtable.fn_cleanup = elem_cleanup;
  register_type(get_type_id(hashmap_custom_t), &vtable);
}

INITIALIZER(register_hashmap_cptr)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_is_equal = key_equal_str;
  vtable.fn_hash = hash_calc_str;
  vtable.fn_replicate = replicate_str;
  vtable.fn_cleanup = cleanup_str;
  register_type(get_type_id(cptr), &vtable);
}

INITIALIZER(register_hashmap_u64)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_is_equal = key_equal;
  vtable.fn_hash = hash_calc;
  register_type(get_type_id(u64), &vtable);
}


static
void
test_chashmap_serialize(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  std::string name = "joe_";

  const u64 total = 10;

  chashmap_t students, copy;
  chashmap_def(&students);
  chashmap_def(&copy);
  chashmap_setup(
    &students,
    get_type_data(u64),
    get_type_data(student_t),
    allocator, 0.6f);
  for (u64 i = 0; i < total; ++i) {
    student_t elem;
    student_def(&elem);
    elem.age = i;
    elem.allocator = allocator;
    name = "joe_" + std::to_string(i);
    set_student_name(&elem, name.c_str());
    chashmap_insert(&students, i, u64, elem, student_t);
  }

  binary_stream_t stream;
  binary_stream_def(&stream);
  binary_stream_setup(&stream, allocator);

  chashmap_serialize(&students, &stream);
  chashmap_deserialize(&copy, allocator, &stream);

  for (u64 i = 0; i < total; ++i) {
    student_t* elem0, *elem1;
    chashmap_at(&students, i, u64, student_t, elem0);
    chashmap_at(&copy, i, u64, student_t, elem1);
    if (!student_is_equal(elem0, elem1))
      assert(false && "has to be equal!");
  }

  chashmap_cleanup(&students, NULL);
  chashmap_cleanup(&copy, NULL);
  binary_stream_cleanup(&stream);
}

void
test_chashmap_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  // test_chashmap_def(allocator, tabs + 1);                     NEWLINE;
  // test_chashmap_basics(allocator, tabs + 1);                  NEWLINE;
  test_chashmap_def_basics(allocator, tabs + 1);              NEWLINE;
  test_chashmap_def_basics_with_macros(allocator, tabs + 1);  NEWLINE;
  // test_chashmap_ops(allocator, tabs + 1);                     NEWLINE;
  // test_chashmap_misc(allocator, tabs + 1);                    NEWLINE;
  // test_chashmap_mem(allocator, tabs + 1);                     NEWLINE;
  // test_chashmap_iterators(allocator, tabs + 1);               NEWLINE;
  // test_chashmap_custom(allocator, tabs + 1);                  NEWLINE;
  // test_chashmap_serialize(allocator, tabs + 1);               NEWLINE;
}