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
#include <cassert>
#include <cstdint>
#include <classroom.h>
#include <common.h>
#include <library/allocator/allocator.h>
#include <library/core/core.h>
#include <library/streams/binary_stream.h>
#include <library/type_registry/type_registry.h>


static
void
serialize_generic(type_id_t type, void *ptr, binary_stream_t *stream)
{
  vtable_t *vtable;
  binary_stream_write(stream, &type, sizeof(type_id_t));
  vtable = get_vtable(type);
  vtable->fn_serialize(ptr, stream);
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

  vtable_t *vtable = get_vtable(type);
  *ptr = allocator->mem_alloc(vtable->fn_type_size());
  vtable->fn_deserialize(*ptr, allocator, stream);
}

static
void
print_student(
  student_t &student,
  const int32_t tabs,
  const bool omit_function_header = false)
{
  if (!omit_function_header)
    PRINT_FUNCTION;

  CTABS << "student(age:" << student.age << ", name:" << student.name << ")";
  NEWLINE;
}

static
void
print_classroom(classroom_t &classroom, const int32_t tabs)
{
  PRINT_FUNCTION;

  CTABS << "classroom{";
  NEWLINE;
  for (uint32_t i = 0; i < classroom.count; ++i)
    print_student(classroom.students[i], tabs + 1, true);
  CTABS << "}";
  NEWLINE;
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
  set_student_name(&mario, "super mario odyssey!");
  set_student_name(&luigi, "luigi land 2");
  set_student_name(&peach, "peach dressup adventure!");
  set_student_name(&bowser, "marry me daisy!");

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

  print_student(bowser, tabs);
  print_classroom(classroom, tabs);

  print_student(*(student_t*)elements[0], tabs);
  print_classroom(*(classroom_t*)elements[1], tabs);

  assert(student_is_equal(elements[0], &bowser) && "must be equal!");
  assert(classroom_is_equal(elements[1], &classroom) && "must be equal!");
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
test_registry_macros(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;
  uint64_t type_data1 = get_type_data(uint8_t);
  uint64_t type_data2 = pack_type_data(get_type_id(uint8_t), sizeof(uint8_t));
  assert(type_data1 == type_data2);
  uint32_t type_id1 = get_type_id(uint8_t);
  uint32_t type_id2 = get_type_id_from_data(type_data1);
  assert(type_id1 == type_id2);
  uint32_t type_size1 = sizeof(uint8_t);
  uint32_t type_size2 = get_type_size_from_data(type_data1);
  assert(type_size1 == type_size2);
}

void
test_registry_main(const allocator_t* allocator, const int32_t tabs)
{
  PRINT_FUNCTION;

  test_registry_macros(allocator, tabs + 1);   NEWLINE;
  test_registry(allocator, tabs + 1);          NEWLINE;
}