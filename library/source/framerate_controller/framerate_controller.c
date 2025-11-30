/**
 * @file framerate_controller.c
 * @author khalilhenoud@gmail.com
 * @brief
 * @version 0.1
 * @date 2024-01-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <assert.h>
#include <math.h>
#include <library/allocator/allocator.h>
#include <library/framerate_controller/framerate_controller.h>
#include <library/os/os.h>


typedef
struct framerate_controller_internal_t {
  double current_fps;

  // cast to DWORD when working with timegettime.
  uint64_t start;
  uint64_t end;
  uint64_t target_frame_ms;

  // performance counter specific
  uint64_t ticks_per_second;
  uint32_t freq_counters_supported;
} framerate_controller_internal_t;

// make this opaque
typedef
struct framerate_controller_t {
  uint64_t target_fps;
  uint32_t locked;
  framerate_controller_internal_t internal;
} framerate_controller_t;

inline
uint32_t
is_zero(double val)
{
  double eps = 1e-6;
  return fabs(val) <= eps;
}

inline
void
populate_at_start(framerate_controller_t *controller)
{
  assert(controller);

  if (controller->internal.freq_counters_supported) {
    controller->internal.freq_counters_supported =
      get_performance_frequency(&controller->internal.ticks_per_second);
    get_performance_counter(&controller->internal.start);
  } else
    controller->internal.start = time_get_time();
}

inline
void
populate_at_end(framerate_controller_t *controller)
{
  assert(controller);

  if (controller->internal.freq_counters_supported)
    get_performance_counter(&controller->internal.end);
  else
    controller->internal.end = time_get_time();
}

inline
double
get_cycle_duration(
  framerate_controller_t *controller,
  uint64_t start,
  uint64_t end,
  framerate_duration_t format)
{
  assert(controller);

  {
    double mult = format == FPS_DURATION_MS ? 1000. : 1.;
    if (controller->internal.freq_counters_supported) {
      uint64_t delta_ticks = end - start;
      return (double)delta_ticks / controller->internal.ticks_per_second * mult;
    } else {
      uint64_t delta_ms = end - start;
      return (double)delta_ms / 1000. * mult;
    }
  }
}

static
void
initialize_controller(
  framerate_controller_t *controller,
  uint64_t target_fps,
  uint32_t locked)
{
  assert(controller);

  controller->target_fps = target_fps;
  controller->locked = locked;

  controller->internal.ticks_per_second = 0;
  controller->internal.freq_counters_supported =
    get_performance_frequency(&controller->internal.ticks_per_second);

  populate_at_start(controller);

  controller->internal.current_fps = 0.;
  controller->internal.target_frame_ms = locked ? (1000ull / target_fps) : 1ull;
}

framerate_controller_t *
controller_allocate(
  const allocator_t *allocator,
  uint64_t target_fps,
  uint32_t locked)
{
  framerate_controller_t *controller = allocator->mem_alloc(
    sizeof(framerate_controller_t));
  initialize_controller(controller, target_fps, locked);
  return controller;
}

void
controller_free(
  framerate_controller_t *controller,
  const allocator_t *allocator)
{
  assert(controller);
  allocator->mem_free(controller);
}

float
controller_start(framerate_controller_t *controller)
{
  assert(controller);

  {
    uint64_t prior_start = controller->internal.start;
    populate_at_start(controller);
    return get_cycle_duration(
      controller,
      prior_start,
      controller->internal.start,
      FPS_DURATION_SECONDS);
  }
}

double
controller_end(framerate_controller_t *controller)
{
  assert(controller);

  {
    double duration_ms = 1.;
    double previous_fps = controller->internal.current_fps;
    populate_at_end(controller);
    duration_ms =
      get_cycle_duration(
        controller,
        controller->internal.start,
        controller->internal.end,
        FPS_DURATION_MS);
    duration_ms = is_zero(duration_ms) ? 1. : duration_ms;
    controller->internal.current_fps = 1000. / duration_ms;

    if (
      controller->locked &&
      controller->internal.target_frame_ms > (duration_ms + BUFFER_TIME_MS)) {
      double ms_sleep =
        controller->internal.target_frame_ms - (duration_ms + BUFFER_TIME_MS);

      sleep((uint64_t)ms_sleep);
    }

    return previous_fps;
  }
}

double
get_current_fps(framerate_controller_t *controller)
{
  assert(controller);
  return controller->internal.current_fps;
}

void
lock_fps(
  framerate_controller_t *controller,
  uint64_t target_fps)
{
  assert(controller);
  controller->locked = 1u;
  controller->target_fps = target_fps;
  controller->internal.target_frame_ms = 1000ull / target_fps;
}

void
unlock_fps(framerate_controller_t *controller)
{
  assert(controller);
  controller->locked = 0u;
}