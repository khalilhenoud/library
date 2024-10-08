/**
 * @file fnv.inl
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2024-09-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LIB_HASH_FNV_H
#define LIB_HASH_FNV_H


inline
uint32_t
hash_fnv1a_32(const void* bytes, size_t length)
{
  uint32_t hash = FNV32_OFFSET_BASIS;
  size_t i = 0;

  for (; i < length; ++i) {
    hash ^= ((unsigned char *)bytes)[i];
    hash *= FNV32_PRIME;
  }
  
  return hash;
}

inline
uint64_t
hash_fnv1a_64(const void* bytes, size_t length)
{
  uint64_t hash = FNV64_OFFSET_BASIS;
  size_t i = 0;

  for (; i < length; ++i) {
    hash ^= ((unsigned char *)bytes)[i];
    hash *= FNV64_PRIME;
  }

  return hash;
}

#endif