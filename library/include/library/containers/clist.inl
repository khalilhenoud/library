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
  type_data_t type_data, 
  const allocator_t* allocator)
{
  assert(list && allocator);

  {
    list->elem_data = get_cont_elem_data_from_packed(type_data);
    list->size = 0;
    list->allocator = allocator;
    list->nodes = NULL;
  }
}

inline
void
clist_cleanup(void *ptr, const allocator_t* allocator)
{
  clist_t *list = (clist_t *)ptr;
  assert(list && !clist_is_def(list));
  assert(!allocator && "this type owns its own allocator!");

  while (list->size)
    clist_erase(list, 0);
  
  elem_data_clear(&list->elem_data);
  list->size = 0;
  list->allocator = NULL;
  list->nodes = NULL;
}

inline
void 
clist_replicate(
  const void *v_src, 
  void *v_dst, 
  const allocator_t *allocator)
{
  const clist_t *src = (const clist_t *)v_src;
  clist_t *dst = (clist_t *)v_dst;
  assert(src && !clist_is_def(src));
  assert(dst);
  assert(
    (clist_is_def(dst) && allocator) || 
    (
      !clist_is_def(dst) && 
      !allocator && 
      elem_data_identical(&dst->elem_data, &src->elem_data) && 
      dst->size == 0));

  if (clist_is_def(dst)) {
    clist_setup(
      dst, 
      pack_type_data(src->elem_data.type_id, src->elem_data.size), allocator);
  }

  {
    const clist_node_t* src_node;
    clist_node_t* dst_node;
    size_t i = 0;
    for (; i < src->size; ++i) {
      fn_replicate_t replicate = elem_data_get_replicate_fn(&src->elem_data);
      src_node = clist_at_cst(src, i);
      clist_insert_empty(dst, i);
      dst_node = clist_at(dst, i);

      if (!replicate)
        memcpy(dst_node->data, src_node->data, src->elem_data.size);
      else
        replicate(src_node->data, dst_node->data, dst->allocator);
    }
  }
}

inline
void
clist_fullswap(clist_t* src, clist_t* dst)
{
  assert(src && dst);
  {
    clist_t tmp = *src;
    *src = *dst;
    *dst = tmp;
  }
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
  return list->elem_data.size; 
}

inline
const allocator_t*
clist_allocator(const clist_t* list) 
{ 
  assert(list && !clist_is_def(list)); 
  return list->allocator; 
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
const clist_node_t* 
clist_at_cst(const clist_t *list, size_t index)
{
  assert(list && !clist_is_def(list));
  assert(index <= list->size);

  {
    const clist_node_t* start = list->nodes;
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
    fn_cleanup_t cleanup = elem_data_get_cleanup_fn(&list->elem_data);
    
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

    if (cleanup)
      cleanup(target->data, list->allocator);
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
  assert(list);
  {
    clist_iterator_t iter;
    iter.list = list;
    iter.current = list->nodes;
    iter.last = list->nodes ? list->nodes->previous : NULL;
    return iter;
  }
}

inline
clist_iterator_t
clist_end(clist_t* list)
{
  assert(list);
  { 
    clist_iterator_t iter;
    iter.list = list;
    iter.current = iter.last = NULL;
    return iter;
  }
}

inline
int32_t
clist_iter_equal(clist_iterator_t left, clist_iterator_t right)
{
  return 
    left.list == right.list && 
    left.last == right.last && 
    left.current == right.current;
}

inline
void
clist_advance(clist_iterator_t* iter)
{
  assert(iter);
  if (iter->current == iter->last)
    iter->current = iter->last = NULL;
  else
    iter->current = iter->current->next;
}

#endif