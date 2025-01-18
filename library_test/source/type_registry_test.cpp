/**
 * @file type_registry_test.cpp
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-01-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <library/allocator/allocator.h>
#include <library/streams/binary_stream.h>
#include <library/type_registry/type_registry.h>
#include <library/core/core.h>
#include <cstdint>
#include <cassert>
#include <common.h>


typedef
struct student_t {
  char *name;                   // c style, null terminated
  uint32_t length;              // result of strlen
  uint32_t age;
  const allocator_t *allocator;
} student_t;

void
student_def(void *ptr)
{
  assert(ptr);
  {
    student_t *typed = (student_t*)ptr;
    typed->name = NULL;
    typed->length = 0;
    typed->age = 0;
    typed->allocator = NULL;
  }
}

uint32_t 
student_is_def(const void *ptr)
{
  assert(ptr);
  {
    const student_t *typed = (const student_t *)ptr;
    return 
      typed->name == NULL && 
      typed->length == 0 && 
      typed->age == 0 && 
      typed->allocator == NULL;
  }
}

uint32_t 
student_is_equal(const void *lhs, const void *rhs)
{
  assert(lhs && rhs);

  {
    const student_t *left = (const student_t *)lhs;
    const student_t *right = (const student_t *)rhs;
    return 
      !strcmp(left->name, right->name) && 
      left->length == right->length &&
      left->age == right->age;
  }
}

void 
student_fullswap(void* lhs, void* rhs)
{
  assert(lhs && rhs);

  {
    student_t *left = (student_t *)lhs;
    student_t *right = (student_t *)rhs;
    student_t tmp = *right;
    right->age = left->age;
    right->allocator = left->allocator;
    right->length = left->length;
    right->name = left->name;
    left->age = tmp.age;
    left->allocator = tmp.allocator;
    left->length = tmp.length;
    left->name = tmp.name;
  }
}

void 
student_serialize(const void *src, binary_stream_t* stream)
{
  assert(src && stream);

  const student_t *student = (const student_t *)src;

  binary_stream_write(stream, &student->age, sizeof(uint32_t));
  binary_stream_write(stream, &student->length, sizeof(uint32_t));
  binary_stream_write(stream, student->name, (size_t)student->length);
}

void 
student_deserialize(
  void *dst, 
  const allocator_t *allocator, 
  binary_stream_t* stream)
{
  assert(dst && allocator && stream);

  size_t s_u32 = sizeof(uint32_t);
  student_t *student = (student_t *)dst;
  student->allocator = allocator;

  binary_stream_read(stream, (uint8_t *)&student->age, s_u32, s_u32);
  binary_stream_read(stream, (uint8_t *)&student->length, s_u32, s_u32);
  student->name = (char *)allocator->mem_alloc(student->length + 1);
  memset(student->name, 0, student->length + 1);
  
  binary_stream_read(
    stream, (uint8_t *)student->name, student->length + 1, student->length);
}

size_t 
student_size()
{
  return sizeof(student_t);
}

uint32_t 
student_owns_alloc()
{
  return 1;
}

const allocator_t* 
student_get_alloc(const void *ptr)
{
  assert(ptr);

  {
    const student_t *typed = (const student_t *)ptr;
    return typed->allocator;
  }
}

void 
student_cleanup(void *ptr, const allocator_t* allocator)
{
  // student contains its own allocator, the passed one has to be null.
  assert(ptr && !allocator);

  {
    student_t *typed = (student_t *)ptr;
    typed->allocator->mem_free(typed->name);
  }
}

INITIALIZER(register_student)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = student_def;
  vtable.fn_is_def = student_is_def;
  vtable.fn_is_equal = student_is_equal;
  vtable.fn_fullswap = student_fullswap;
  vtable.fn_serialize = student_serialize;
  vtable.fn_deserialize = student_deserialize;
  vtable.fn_size = student_size;
  vtable.fn_owns_alloc = student_owns_alloc;
  vtable.fn_get_alloc = student_get_alloc;
  vtable.fn_cleanup = student_cleanup;
  register_type(get_type_id(student_t), &vtable);
}

typedef 
struct {
  uint32_t count;
  student_t *students;
} classroom_t;

void
classroom_def(void *ptr)
{
  assert(ptr);
  {
    classroom_t *typed = (classroom_t*)ptr;
    typed->count = 0;
    typed->students = NULL;
  }
}

uint32_t 
classroom_is_def(const void *ptr)
{
  assert(ptr);
  {
    const classroom_t *typed = (const classroom_t *)ptr;
    return 
      typed->count == 0 && 
      typed->students == NULL;
  }
}

uint32_t 
classroom_is_equal(const void *lhs, const void *rhs)
{
  assert(lhs && rhs);

  {
    const classroom_t *left = (const classroom_t *)lhs;
    const classroom_t *right = (const classroom_t *)rhs;
    if (left->count != right->count)
      return 0;

    for (uint32_t i = 0; i < left->count; ++i) {
      if (!student_is_equal(left->students + i, right->students + i))
        return 0;
    }

    return 1;
  }
}

void 
classroom_serialize(const void *src, binary_stream_t* stream)
{
  assert(src && stream);

  const classroom_t *classroom = (const classroom_t *)src;

  binary_stream_write(stream, &classroom->count, sizeof(uint32_t));
  for (uint32_t i = 0; i < classroom->count; ++i)
    student_serialize(classroom->students + i, stream);
}

void 
classroom_deserialize(
  void *dst, 
  const allocator_t *allocator, 
  binary_stream_t* stream)
{
  assert(dst && allocator && stream);

  size_t s_u32 = sizeof(uint32_t);
  classroom_t *classroom = (classroom_t *)dst;

  binary_stream_read(stream, (uint8_t *)&classroom->count, s_u32, s_u32);
  classroom->students = (student_t *)allocator->mem_alloc(
    sizeof(student_t) * classroom->count);
  for (uint32_t i = 0; i < classroom->count; ++i)
    student_deserialize(classroom->students + i, allocator, stream);
}

size_t 
classroom_size()
{
  return sizeof(classroom_t);
}

uint32_t 
classroom_owns_alloc()
{
  return 0;
}

const allocator_t* 
classroom_get_alloc(const void *ptr)
{
  assert(ptr);
  return NULL;
}

void 
classroom_cleanup(void *ptr, const allocator_t* allocator)
{
  assert(ptr && allocator);

  {
    classroom_t *room = (classroom_t *)ptr;
    for (uint32_t i = 0; i < room->count; ++i)
      student_cleanup(
        room->students + i, NULL);

    allocator->mem_free(room->students);
  }
}

INITIALIZER(register_classroom)
{
  vtable_t vtable;
  memset(&vtable, 0, sizeof(vtable_t));
  vtable.fn_def = classroom_def;
  vtable.fn_is_def = classroom_is_def;
  vtable.fn_is_equal = classroom_is_equal;
  vtable.fn_serialize = classroom_serialize;
  vtable.fn_deserialize = classroom_deserialize;
  vtable.fn_size = classroom_size;
  vtable.fn_owns_alloc = classroom_owns_alloc;
  vtable.fn_get_alloc = classroom_get_alloc;
  vtable.fn_cleanup = classroom_cleanup;
  register_type(get_type_id(classroom_t), &vtable);
}

static
void 
set_name(student_t &student, const char *name)
{
  student.length = strlen(name);
  student.name = (char *)student.allocator->mem_alloc(student.length + 1);
  memcpy(student.name, name, student.length + 1);
}

static
void
serialize_generic(type_id_t type, void *ptr, binary_stream_t *stream)
{
  vtable_t vtable;
  binary_stream_write(stream, &type, sizeof(type_id_t));
  get_vtable(type, &vtable);
  vtable.fn_serialize(ptr, stream);
}

static
void
deserialize_generic(
  void **ptr, 
  const allocator_t* allocator, 
  binary_stream_t *stream)
{
  type_id_t type;
  size_t tsize = sizeof(type_id_t);
  binary_stream_read(stream, (uint8_t *)&type, tsize, tsize);

  vtable_t vtable;
  get_vtable(type, &vtable);
  *ptr = allocator->mem_alloc(vtable.fn_size());
  vtable.fn_deserialize(*ptr, allocator, stream);
}

void
test_registry(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  student_t mario, luigi, peach, bowser;
  student_def(&mario), student_def(&luigi), student_def(&peach), 
  student_def(&bowser);
  mario.allocator = luigi.allocator = peach.allocator = bowser.allocator = 
  allocator;
  
  mario.age = 10; luigi.age = 11; peach.age = 9; bowser.age = 15;
  set_name(mario, "super mario odyssey!");
  set_name(luigi, "luigi land 2");
  set_name(peach, "peach dressup adventure!");
  set_name(bowser, "marry me daisy!");

  classroom_t classroom;
  classroom_def(&classroom);
  classroom.count = 3;
  classroom.students = (student_t *)allocator->mem_alloc(
    sizeof(student_t) * classroom.count);
  memset(classroom.students, 0, sizeof(student_t) * classroom.count);
  student_fullswap(classroom.students + 0, &mario);
  student_fullswap(classroom.students + 1, &luigi);
  student_fullswap(classroom.students + 2, &peach);

  binary_stream_t stream;
  binary_stream_def(&stream);
  binary_stream_setup(&stream, allocator);

  // bowser + classroom { mario, luigi, peach } = 2 elements.
  const uint32_t entries = 2; 
  void* elements[entries] = { NULL, NULL };
  serialize_generic(get_type_id(student_t), &bowser, &stream);
  serialize_generic(get_type_id(classroom_t), &classroom, &stream);
  deserialize_generic(elements + 0, allocator, &stream);
  deserialize_generic(elements + 1, allocator, &stream);

  assert(student_is_equal(elements[0], &bowser) && "must be equal!");
  assert(classroom_is_equal(elements[1], &classroom) && "must be equal!");
  CTABS << "serialized elements are identical to deserialized ones";
  NEWLINE;

  student_cleanup(elements[0], NULL);
  allocator->mem_free(elements[0]);
  classroom_cleanup(elements[1], allocator);
  allocator->mem_free(elements[1]);

  student_cleanup(&bowser, NULL);
  classroom_cleanup(&classroom, allocator);
  
  assert(stream.pos == STREAM_EOF);
  binary_stream_cleanup(&stream);
}

void
test_registry_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_registry(allocator, tabs + 1);          NEWLINE;
}