/**
 * @file clist.inl
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-08-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_LIST_INL
#define LIB_LIST_INL


inline
void
clist_setup(
  clist_t* list, 
  size_t elem_size, 
  const allocator_t* allocator, 
  clist_elem_cleanup_t elem_cleanup_fn)
{
  assert(list && allocator);

  {
    list->elem_size = elem_size;
    list->size = 0;
    list->allocator = allocator;
    list->elem_cleanup = elem_cleanup_fn;
    list->nodes = NULL;
  }
}

inline
void
clist_cleanup(clist_t* list)
{
  assert(list && !clist_is_def(list));

  while (list->size)
    clist_erase(list, 0);
  
  list->elem_size = list->size = 0;
  list->allocator = NULL;
  list->elem_cleanup = NULL;
  list->nodes = NULL;
}

inline
size_t
clist_size(const clist_t* list) 
{ 
  assert(list && !clist_is_def(list)); 
  return list->size;
}

inline
size_t
clist_elem_size(const clist_t* list) 
{ 
  assert(list && !clist_is_def(list));  
  return list->elem_size; 
}

inline
const allocator_t*
clist_allocator(const clist_t* list) 
{ 
  assert(list && !clist_is_def(list)); 
  return list->allocator; 
} 

inline
clist_elem_cleanup_t
clist_elem_cleanup(const clist_t* list) 
{ 
  assert(list && !clist_is_def(list)); 
  return list->elem_cleanup; 
} 

inline
int32_t
clist_empty(const clist_t* list) 
{ 
  assert(list && !clist_is_def(list)); 
  return list->size == 0;
}

inline
clist_node_t* 
clist_at(clist_t *list, size_t index)
{
  assert(list && !clist_is_def(list));
  assert(index <= list->size);

  {
    clist_node_t* start = list->nodes;
    int64_t i = (int64_t)index;
    while (i--)
      start = start->next;
    return start;
  }
}

inline
void
clist_erase(clist_t* list, size_t index)
{
  assert(list && !clist_is_def(list));
  assert(index < list->size);

  {
    clist_node_t* target = clist_at(list, index);
    clist_node_t* previous = target->previous;
    clist_node_t* next = target->next;
    
    // 1, 2, many nodes scenarios.
    if (previous == target)
      list->nodes = NULL;
    else if (previous == next)
      list->nodes = previous->next = previous->previous = previous;
    else {
      previous->next = next;
      previous->next->previous = previous;
      list->nodes = (list->nodes == target) ? next : list->nodes;
    }

    if (list->elem_cleanup)
      list->elem_cleanup(target->data, list->allocator);
    list->allocator->mem_free(target->data);
    list->allocator->mem_free(target);
    --list->size;
  }
}

inline
void
clist_clear(clist_t* list)
{
  assert(list && !clist_is_def(list));

  while (list->size)
    clist_erase(list, 0);
}

inline
void
clist_pop_back(clist_t* list)
{
  assert(list && !clist_is_def(list));
  assert(list->size);

  clist_erase(list, list->size - 1);
}

inline
void
clist_pop_front(clist_t* list)
{
  assert(list && !clist_is_def(list));
  assert(list->size);

  clist_erase(list, 0);
}

inline
clist_iterator_t
clist_begin(clist_t* list)
{
  clist_iterator_t iter;
  iter.list = list;
  iter.current = list->nodes;
  iter.last = list->nodes ? list->nodes->previous : NULL;
}

inline
clist_iterator_t
clist_end(clist_t* list)
{
  clist_iterator_t iter;
  iter.list = list;
  iter.current = iter.last = NULL;
}

inline
void
clist_advance(clist_iterator_t* iter)
{
  if (iter->current == iter->last)
    iter->current = iter->last = NULL;
  else
    iter->current = iter->current->next;
}

#endif