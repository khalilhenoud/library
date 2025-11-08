/**
 * @file precision_timers.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2025-11-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef LIB_PRECISION_TIMERS_H
#define LIB_PRECISION_TIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


void
set_periodic_timers_resolution(uint32_t ms);

void
end_periodic_timers_resolution(uint32_t ms);

#include "precision_timers.impl"

#ifdef __cplusplus
}
#endif

#endif