/**
 * @file classroom.h
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2025-02-08
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef CLASSROOM_H
#define CLASSROOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


typedef struct binary_stream_t binary_stream_t;
typedef struct allocator_t allocator_t;

typedef
struct student_t {
  char *name;                   // c style, null terminated
  uint32_t length;              // result of strlen
  uint32_t age;
  const allocator_t *allocator;
} student_t;

void
student_def(void *ptr);

uint32_t
student_is_def(const void *ptr);

uint32_t
student_is_equal(const void *lhs, const void *rhs);

void
student_fullswap(void* lhs, void* rhs);

void
student_serialize(const void *src, binary_stream_t* stream);

void
student_deserialize(
  void *dst,
  const allocator_t *allocator,
  binary_stream_t* stream);

size_t
student_size(void);

uint32_t
student_owns_alloc(void);

const allocator_t*
student_get_alloc(const void *ptr);

void
student_cleanup(void *ptr, const allocator_t* allocator);

void
set_student_name(student_t *student, const char *name);

typedef
struct {
  uint32_t count;
  student_t *students;
} classroom_t;

void
classroom_def(void *ptr);

uint32_t
classroom_is_def(const void *ptr);

uint32_t
classroom_is_equal(const void *lhs, const void *rhs);

void
classroom_serialize(const void *src, binary_stream_t* stream);

void
classroom_deserialize(
  void *dst,
  const allocator_t *allocator,
  binary_stream_t* stream);

size_t
classroom_size(void);

uint32_t
classroom_owns_alloc(void);

const allocator_t*
classroom_get_alloc(const void *ptr);

void
classroom_cleanup(void *ptr, const allocator_t* allocator);

#ifdef __cplusplus
}
#endif

#endif