/**
 * @file types.h
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2026-04-11
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef C_LIB_ASSETS_TYPES_H
#define c_LIB_ASSETS_TYPES_H


typedef struct allocator_t allocator_t;
typedef struct asset_ref_t asset_ref_t;

typedef
void (*loader_t)(void **, const asset_ref_t *, const allocator_t *);
typedef
void (*deloader_t)(void **, const asset_ref_t *, const allocator_t *);

#endif