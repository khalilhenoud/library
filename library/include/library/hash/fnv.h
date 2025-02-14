/**
 * @file fnv.h
 * @author khalilhenoud@gmail.com
 * @brief fnv1a 32 bit and 64 bit version
 * @version 0.1
 * @date 2024-09-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HASH_FNV_H
#define HASH_FNV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FNV32_PRIME           0x01000193
#define FNV32_OFFSET_BASIS    0x811c9dc5
#define FNV64_PRIME           0x00000100000001B3
#define FNV64_OFFSET_BASIS    0xcbf29ce484222325


uint32_t
hash_fnv1a_32(const void* bytes, size_t length);

uint64_t
hash_fnv1a_64(const void* bytes, size_t length);

#include "fnv.impl"

#ifdef __cplusplus
}
#endif

#endif