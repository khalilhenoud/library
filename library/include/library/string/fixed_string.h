/**
 * @file fixed_string.h
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2023-04-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef FIXED_STRING_H
#define FIXED_STRING_H


typedef
struct fixed_str_t {
  char data[512];
} fixed_str_t;

typedef
struct fixed_str_128_t {
  char data[128];
} fixed_str_128_t;

#endif