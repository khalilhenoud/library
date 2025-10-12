/**
 * @file clist.h
 * @author khalilhenoud@gmail.com
 * @brief generic list
 * @version 0.1
 * @date 2024-08-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_LIST_H
#define LIB_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <library/allocator/allocator.h>
#include <library/type_registry/type_registry.h>


////////////////////////////////////////////////////////////////////////////////
//| clist_t, * = clist
//|=============================================================================
//| OPERATION                   | SUPPORTED
//|=============================================================================
//|    *_def                    | YES
//|    *_is_def                 | YES
//|    *_replicate              | YES
//|    *_fullswap               | YES
//|    *_serialize              | YES
//|    *_deserialize            | YES
//|    *_hash                   |
//|    *_is_equal               |
//|    *_type_size              | YES
//|    *_type_alignment         |
//|    *_type_id_count          | YES
//|    *_type_ids               | YES
//|    *_owns_alloc             | YES
//|    *_get_alloc              | YES
//|    *_cleanup                | YES
////////////////////////////////////////////////////////////////////////////////
// TODO:
//  - mem_alloc for every node is not ideal, provide a better implementation.
////////////////////////////////////////////////////////////////////////////////

typedef struct binary_stream_t binary_stream_t;
typedef struct clist_node_t clist_node_t;
typedef struct clist_t clist_t;

typedef
struct clist_node_t {
  clist_node_t *next;
  clist_node_t *previous;
  void *data;
} clist_node_t;

typedef
struct clist_iterator_t {
  clist_t *list;
  clist_node_t *current;
  clist_node_t *last;
} clist_iterator_t; 

typedef
struct clist_t {
  container_elem_data_t elem_data;
  size_t size;
  const allocator_t* allocator;
  clist_node_t *nodes;
} clist_t;

inline
void 
clist_def(void *ptr)
{
  assert(ptr);
  memset(ptr, 0, sizeof(clist_t));
}

inline
uint32_t 
clist_is_def(const void *ptr)
{
  const clist_t *list = (const clist_t *)ptr;
  clist_t def;
  clist_def(&def);
  return 
    elem_data_identical(&list->elem_data, &def.elem_data) && 
    list->size == def.size && 
    list->allocator == def.allocator &&
    list->nodes == def.nodes;
}

/** 
 * NOTE: will assert if 'src' is not initialized, or if 'dst' is initialized but
 * with non-zero size.
 * NOTE: either 'dst' is def or 'allocator' is NULL, not both (we assert).
 */
void 
clist_replicate(
  const void *src, 
  void *dst, 
  const allocator_t *allocator);

void
clist_fullswap(void* src, void* dst);

void 
clist_serialize(
  const void *src, 
  binary_stream_t* stream);

void 
clist_deserialize(
  void *dst, 
  const allocator_t *allocator, 
  binary_stream_t* stream);

inline
size_t 
clist_type_size(void)
{
  return sizeof(clist_t);
}

inline
uint32_t 
clist_type_id_count(void)
{
  return 1;
}

inline
void 
clist_type_ids(
  const void *src, 
  type_id_t *ids)
{
  assert(src && ids);
  ids[0] = ((const clist_t *)src)->elem_data.type_id;
}

inline
uint32_t 
clist_owns_alloc(void)
{
  return 1;
}

inline
const allocator_t*
clist_get_alloc(const void *list)
{
  assert(list);
  return ((const clist_t *)list)->allocator;
}

void
clist_cleanup(void *ptr, const allocator_t* allocator);

////////////////////////////////////////////////////////////////////////////////
void
clist_setup(
  clist_t* list, 
  type_data_t type_data, 
  const allocator_t* allocator);

size_t
clist_size(const clist_t* vec);

size_t
clist_elem_size(const clist_t* vec);

int32_t
clist_empty(const clist_t* vec);

/** returns the node at index, internal use only. */
clist_node_t* 
clist_at(clist_t *list, size_t index);
const clist_node_t* 
clist_at_cst(const clist_t *list, size_t index);

/** removes the node at index from the list. */
void
clist_erase(clist_t* list, size_t index);

/** 
 * erases all data in the list. 
 * NOTE: allocator and elem_cleanup are maintained, meaning the container is 
 * still valid after clear is called.
 */
void
clist_clear(clist_t* list);

/** removes the last element from the list */
void
clist_pop_back(clist_t* list);

/** removes the first element from the list */
void
clist_pop_front(clist_t* list);

/** returns an iterator that can be used to iterate over the list. */
clist_iterator_t
clist_begin(clist_t* list);

/** returns an end iterator. */
clist_iterator_t
clist_end(clist_t* list);

/** advance the iterator. */
void
clist_advance(clist_iterator_t* iter);

/** 1 if equal, 0 otherwise */
int32_t
clist_iter_equal(clist_iterator_t left, clist_iterator_t right);

////////////////////////////////////////////////////////////////////////////////
/** helper functionality to speed up setup process */
#define clist_setup2(list__, type__)                                    \
  do {                                                                  \
    clist_def((list__));                                                \
    clist_setup((list__), get_type_data(type__), &g_default_allocator); \
  } while (0)

#define clist_cleanup2(list__)  \
  clist_cleanup((list__), NULL)

#define clist_deref(iter, type) \
  (type*)((iter)->current->data)

#define clist_as(list, n, type) \
  (type*)(clist_at((list), (n))->data)

#define clist_front(list, type) \
  (type*)(clist_at((list), 0)->data)

#define clist_back(list, type) \
  (type*)(clist_at((list), (list)->size - 1)->data)

/** insert element 'val' at 'pos' in the list */
#define clist_insert(list, pos, val, type)                                    \
  do {                                                                        \
    assert((list) && !clist_is_def(list));                                    \
    assert((pos) >= 0 && (pos) <= (list)->size);                              \
    {                                                                         \
      clist_node_t* to_insert =                                               \
        (clist_node_t*)(list)->allocator->mem_alloc(sizeof(clist_node_t));    \
      to_insert->data = (list)->allocator->mem_alloc((list)->elem_data.size); \
      *((type*)to_insert->data) = (val);                                      \
                                                                              \
      if (!(list)->nodes) {                                                   \
        (list)->nodes = to_insert;                                            \
        to_insert->previous = to_insert->next = to_insert;                    \
      } else {                                                                \
        clist_node_t* current = clist_at((list), (pos));                      \
        clist_node_t* previous = current->previous;                           \
                                                                              \
        if (current == previous) {                                            \
          to_insert->previous = to_insert->next = current;                    \
          current->previous = current->next = to_insert;                      \
        } else {                                                              \
          to_insert->previous = previous;                                     \
          to_insert->next = current;                                          \
          previous->next = to_insert;                                         \
          current->previous = to_insert;                                      \
        }                                                                     \
                                                                              \
        (list)->nodes = (pos) == 0 ? to_insert : (list)->nodes;               \
      }                                                                       \
      ++((list)->size);                                                       \
    }                                                                         \
  } while (0)

#define clist_insert_empty(list, pos)                                         \
  do {                                                                        \
    assert((list) && !clist_is_def(list));                                    \
    assert((pos) >= 0 && (pos) <= (list)->size);                              \
    {                                                                         \
      clist_node_t* to_insert =                                               \
        (clist_node_t*)(list)->allocator->mem_alloc(sizeof(clist_node_t));    \
      to_insert->data = (list)->allocator->mem_alloc((list)->elem_data.size); \
      memset(to_insert->data, 0, (list)->elem_data.size);                     \
                                                                              \
      if (!(list)->nodes) {                                                   \
        (list)->nodes = to_insert;                                            \
        to_insert->previous = to_insert->next = to_insert;                    \
      } else {                                                                \
        clist_node_t* current = clist_at((list), (pos));                      \
        clist_node_t* previous = current->previous;                           \
                                                                              \
        if (current == previous) {                                            \
          to_insert->previous = to_insert->next = current;                    \
          current->previous = current->next = to_insert;                      \
        } else {                                                              \
          to_insert->previous = previous;                                     \
          to_insert->next = current;                                          \
          previous->next = to_insert;                                         \
          current->previous = to_insert;                                      \
        }                                                                     \
                                                                              \
        (list)->nodes = (pos) == 0 ? to_insert : (list)->nodes;               \
      }                                                                       \
      ++((list)->size);                                                       \
    }                                                                         \
  } while (0)

/** adds an element to the end of the list */
#define clist_push_back(list__, value__, type__) \
  clist_insert((list__), (list__)->size, (value__), type__)

/** adds an element to the start of the list */
#define clist_push_front(list__, value__, type__) \
  clist_insert((list__), 0, (value__), type__)

#include "clist.impl"

#ifdef __cplusplus
}
#endif

#endif