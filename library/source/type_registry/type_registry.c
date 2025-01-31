/**
 * @file type_registry.c
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-12-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <library/type_registry/type_registry.h>

#define REGISTRY_TABLE_SIZE 2048
#define REGISTRY_MAX_SIZE 1024
#define ALIASES_LIMIT 256
#define INVALID 0
 

// we allocate twice the size to minimize collisions (same logic as hashmaps).
static
uint32_t key_to_type[REGISTRY_TABLE_SIZE];
static
uint32_t key_to_index[REGISTRY_TABLE_SIZE];

static 
uint8_t vtables[REGISTRY_MAX_SIZE * sizeof(vtable_t)];

static 
uint32_t total_registered;

static
uint32_t total_aliases;

inline
uint32_t
get_shifted_key(const type_id_t type)
{
  return type & ((1 << 11) - 1);
}

inline
uint32_t
increment(const uint32_t key)
{
  return (key + 1) % REGISTRY_TABLE_SIZE;
}

// returns the position of the type or a potential insertion point.
inline
uint32_t
get_key(const type_id_t type)
{
  uint32_t key = get_shifted_key(type);
  while (key_to_type[key] != type) {
    if (key_to_type[key] == INVALID)
      break;

    key = increment(key);
  }

  return key;
}

uint32_t
is_type_registered(const type_id_t type)
{
  return (key_to_type[get_key(type)] == type) ? 1 : 0;
}

void 
register_type(const type_id_t type, const vtable_t *src)
{
  assert(total_registered < REGISTRY_MAX_SIZE && "no more space to register!");
  assert(!is_type_registered(type) && "type is already registered!");

  {
    vtable_t *dst = NULL;
    uint32_t key = get_key(type);
    key_to_type[key] = type;
    key_to_index[key] = total_registered++;
    dst = (vtable_t*)vtables + key_to_index[key];
    *dst = *src;
  }
}

void 
associate_alias(const type_id_t type, const type_id_t alias)
{
  assert(total_aliases < ALIASES_LIMIT && "exceeded aliases limit!");
  assert(is_type_registered(type) && "original type must be registered!");
  assert(!is_type_registered(type) && "alias cannot be registered!");

  ++total_aliases;

  {
    uint32_t key_type = get_key(type);
    uint32_t key_alias = get_key(alias);
    key_to_type[key_alias] = alias;
    key_to_index[key_alias] = key_to_index[key_type];
  }
}

vtable_t * 
get_vtable(const type_id_t type)
{
  assert(is_type_registered(type) && "type has to be registered!");

  {
    uint32_t key = get_key(type);
    uint32_t index = key_to_index[key];
    return (vtable_t *)vtables + index;
  }
}