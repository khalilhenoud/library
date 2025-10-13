/**
 * @file cstring_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-02-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <library/allocator/allocator.h>
#include <library/core/core.h>
#include <library/string/cstring.h>
#include <common.h>
#include <stdint.h>
#include <cassert>
#include <string>


void
print_cstring_content(cstring_t& str, const int32_t tabs)
{
  CTABS << "str: " << (str.length ? str.str : "empty");
  NEWLINE;
}

static
void
print_all(cstring_t& str, const int32_t tabs) 
{
  CTABS << 
  "str(length: " << str.length <<   
  ", str: " << (str.length ? str.str : "empty") <<   
  ", allocator: " << (uint64_t)(str.allocator) <<
  ")" << std::endl;
}

void
test_cstring_def(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cstring_t str; cstring_def(&str);
  print_all(str, tabs);
  CTABS << PRINT_BOOL(cstring_is_def(&str)) << std::endl;
}

void
test_cstring_basics(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("basic tests for cstring");

  cstring_t str;
  cstring_def(&str);
  cstring_setup(&str, "I am here :)", allocator);
  print_all(str, tabs);
  print_cstring_content(str, tabs);
  cstring_cleanup(&str, NULL);
}

void
test_cstring_basics_default_allocator(const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("basic tests for cstring with the default allocator");

  cstring_t *str = cstring_create2("I was here and now I am not :)");
  print_all(*str, tabs);
  print_cstring_content(*str, tabs);
  cstring_free2(str);

  cstring_t str2;
  cstring_setup2(&str2, "This is test number 2!");
  print_all(str2, tabs);
  print_cstring_content(str2, tabs);
  cstring_cleanup2(&str2);
}

void
test_cstring_ops(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  PRINT_DESC("replicate, fullswap, assing, clear, testing");

  {
    CTABS << "replicate testing: " << std::endl;
    cstring_t left; cstring_def(&left);
    cstring_setup(&left, "Allen is here", allocator);
    print_cstring_content(left, tabs);

    cstring_t right; cstring_def(&right);
    cstring_replicate(&left, &right, allocator);
    print_cstring_content(right, tabs);
    cstring_cleanup(&right, NULL);

    cstring_setup(&right, NULL, allocator);
    cstring_replicate(&left, &right, NULL);
    print_cstring_content(right, tabs);
    
    cstring_cleanup(&right, NULL);
    cstring_cleanup(&left, NULL);
  }
  
  {
    CTABS << "fullswap testing: " << std::endl;
    cstring_t left; cstring_def(&left);
    cstring_setup(&left, "Samurai was here", allocator);

    cstring_t right; cstring_def(&right);
    cstring_setup(&right, "Ninja was here", allocator);
    
    print_cstring_content(left, tabs);
    print_cstring_content(right, tabs);

    cstring_fullswap(&left, &right);

    print_cstring_content(left, tabs);
    print_cstring_content(right, tabs);

    cstring_cleanup(&right, NULL);
    cstring_cleanup(&left, NULL);
  }

  {
    CTABS << "clear, assign testing: " << std::endl;
    cstring_t str; cstring_def(&str);
    cstring_setup(&str, "Warrior was here", allocator);
    print_cstring_content(str, tabs);
    cstring_assign(&str, "Xena was here");
    print_cstring_content(str, tabs);
    cstring_clear(&str);
    print_cstring_content(str, tabs);
    cstring_cleanup(&str, NULL);
  }

  {
    CTABS << "hash testing: " << std::endl;
    uint32_t hash = 0;
    cstring_t str; cstring_def(&str);
    cstring_setup(&str, "Warrior was here", allocator);
    print_cstring_content(str, tabs);
    hash = cstring_hash(&str);
    CTABS << "hash: " << hash << std::endl;
    cstring_assign(&str, "Xena was here");
    print_cstring_content(str, tabs);
    hash = cstring_hash(&str);
    CTABS << "hash: " << hash << std::endl;
    cstring_assign(&str, "Warrior was here");
    print_cstring_content(str, tabs);
    hash = cstring_hash(&str);
    CTABS << "hash: " << hash << std::endl;
    cstring_cleanup(&str, NULL);
  }
}

static
void
test_cstring_serialize(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  cstring_t str; cstring_def(&str);
  cstring_t copy; cstring_def(&copy);
  cstring_setup(&str, "This string is to be serialized", allocator);
  print_cstring_content(str, tabs);

  binary_stream_t stream;
  binary_stream_def(&stream);
  binary_stream_setup(&stream, allocator);

  cstring_serialize(&str, &stream);
  cstring_deserialize(&copy, allocator, &stream);
  print_cstring_content(copy, tabs);

  if (!cstring_is_equal(&str, &copy))
    assert(false && "has to be equal!");

  cstring_cleanup(&str, NULL);
  cstring_cleanup(&copy, NULL);
  binary_stream_cleanup(&stream);
}

void
test_cstring_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_cstring_def(allocator, tabs + 1);                        NEWLINE;
  test_cstring_basics(allocator, tabs + 1);                     NEWLINE;
  test_cstring_basics_default_allocator(tabs + 1);              NEWLINE;
  test_cstring_ops(allocator, tabs + 1);                        NEWLINE;
  test_cstring_serialize(allocator, tabs + 1);                  NEWLINE;
}