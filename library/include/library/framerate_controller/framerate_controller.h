/**
 * @file framerate_controller.h
 * @author khalilhenoud@gmail.com
 * @brief 
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef FRAMERATE_CONTROLLER_H
#define FRAMERATE_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <library/internal/module.h>

#define BUFFER_TIME_MS                1


typedef struct allocator_t allocator_t;
typedef struct framerate_controller_t framerate_controller_t;

typedef
enum framerate_duration_t {
  FPS_DURATION_SECONDS,
  FPS_DURATION_MS,
  FPS_DURATION_COUNT
} framerate_duration_t;

LIBRARY_API
framerate_controller_t *
controller_allocate(
  const allocator_t *allocator, 
  uint64_t target_fps, 
  uint32_t locked);

LIBRARY_API
void
controller_free(
  framerate_controller_t *controller,
  const allocator_t *allocator);

LIBRARY_API
float
controller_start(framerate_controller_t *controller);

LIBRARY_API
double
controller_end(framerate_controller_t *controller);

LIBRARY_API
double
get_current_fps(framerate_controller_t *controller);

LIBRARY_API
void
lock_fps(
  framerate_controller_t *controller, 
  uint64_t target_fps);

LIBRARY_API
void
unlock_fps(framerate_controller_t *controller);

#ifdef __cplusplus
}
#endif

#endif
