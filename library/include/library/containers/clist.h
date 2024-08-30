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


// TODO: provide a better implementation for list implementation. I suspect the
// dynamic memory allocation for every node we add is not ideal, that also has
// an effect on our access patterns.

typedef 
void (*clist_elem_cleanup_t)(void *elem_ptr, const allocator_t* allocator);

typedef
void (*clist_elem_replicate_t)(
  const void* src, void* dst, const allocator_t* allocator);

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

// TODO: consider removing size, this nets us a 32 bytes struct.
typedef
struct clist_t {
  size_t elem_size;
  size_t size;
  const allocator_t* allocator;
  clist_elem_cleanup_t elem_cleanup;
  clist_node_t *nodes;
} clist_t;

/** returns a default initialized copy of the struct */
inline
clist_t
clist_def()
{
  clist_t def;
  memset(&def, 0, sizeof(clist_t));
  return def;
}

/** returns 1 if the passed struct is the same as the default */
inline
int32_t
clist_is_def(const clist_t* list)
{
  clist_t def = clist_def();
  return 
    list->elem_size == def.elem_size && 
    list->size == def.size && 
    list->allocator == def.allocator && 
    list->elem_cleanup == def.elem_cleanup && 
    list->nodes == def.nodes;
}

void
clist_setup(
  clist_t* list, 
  size_t elem_size, 
  const allocator_t* allocator, 
  clist_elem_cleanup_t elem_cleanup_fn);

/** frees all internal memory, call setup again to reuse. */
void
clist_cleanup(clist_t* list);

/** 
 * NOTE: will assert if 'src' is not initialized, or if 'dst' is initialized but
 * with non-zero size.
 * NOTE: either 'dst' or 'allocator' is NULL, not both (we assert).
 */
void 
clist_replicate(
  const clist_t *src, 
  clist_t *dst, 
  const allocator_t *allocator, 
  clist_elem_replicate_t elem_replicate_fn);

/** exchanges the content of the specified containers, even allocators */
void
clist_fullswap(clist_t* src, clist_t* dst);

size_t
clist_size(const clist_t* vec);

size_t
clist_elem_size(const clist_t* vec);

const allocator_t*
clist_allocator(const clist_t* vec);

clist_elem_cleanup_t
clist_elem_cleanup(const clist_t* vec);

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


////////////////////////////////////////////////////////////////////////////////
#define clist_iterator clist_iterator_t

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
      to_insert->data = (list)->allocator->mem_alloc((list)->elem_size);      \
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
      to_insert->data = (list)->allocator->mem_alloc((list)->elem_size);      \
      memset(to_insert->data, 0, (list)->elem_size);                          \
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

#include "clist.inl"

#ifdef __cplusplus
}
#endif

#endif