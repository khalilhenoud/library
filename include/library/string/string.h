/**
 * @file string.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-12-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef LIB_STRING_H
#define LIB_STRING_H

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <library/allocator/allocator.h>


typedef
struct string_t {
  char* str;
  // the size of str without the null terminator.
  uint32_t size;
  // the allocator used to allocate the string.
  const allocator_t* allocator;
} string_t;

// TODO: expand the functionality provided here, lots of functions are missing.
// TODO: implement a string builder to avoid constant reallocation.

string_t*
create_string(const char* str, const allocator_t* allocator)
{
  string_t* string = (string_t*)allocator->mem_alloc(sizeof(string_t));
  string->allocator =  allocator;
  string->size = strlen(str);
  string->str = (char*)allocator->mem_alloc(string->size + 1);
  memcpy(string->str, str, string->size + 1);
  return string;
}

// NOTE: do not use string after this operation.
void
free_string(string_t* string)
{
  if (string->str)
    string->allocator->mem_free(string->str);
  // this is safe.
  string->allocator->mem_free(string);
}

// NOTE: keeps the allocator.
uint32_t
clear_string(string_t* string)
{
  uint32_t previous_size = string->size;
  string->size = 0;
  if (string->str)
    string->allocator->mem_free(string->str);
  string->str = NULL;
  return previous_size;
}

uint32_t
assign_string(string_t* string, const char* str)
{
  uint32_t previous_size = clear_string(string);
  string->size = strlen(str);
  string->str = (char*)string->allocator->mem_alloc(string->size + 1);
  memcpy(string->str, str, string->size + 1);
}

// NOTE: copies the string data but keeps the original allocator
void
copy_string(string_t* target, const string_t* source)
{
  clear_string(target);
  assign_string(target, source->str);
}

int32_t
string_equal(const string_t* left, const string_t* right)
{
  return !strcmp(left->str, right->str);
}

#endif