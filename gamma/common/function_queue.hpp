// Copyright (c) 2018-2019 Aleksey Strelnikov
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef GAMMA_COMMON_FUNCTION_QUEUE_HPP_
#define GAMMA_COMMON_FUNCTION_QUEUE_HPP_

#include <vector>

#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "gamma/common/function.hpp"

namespace y {

// A type for executing a collection of callbacks with specified timeouts.
//
// Execution of callbacks is triggered by updating the `FunctionQueue` with
// timestep increments. In other words, real-world time does not apply. This
// allows the calling of functions to be paused, slowed down, or sped up.
//
// This type is thread-safe. It is valid for a callback function to call
// `callAfter()` or `callEvery()`, but not `update()`, on the `FunctionQueue`
// object that stores it.
class FunctionQueue {
 public:
  // Register `f` to be called after at least `delay` time has passed as seen by
  // `update()`. `delay` must be non-negative.
  //
  // A `delay` of zero causes the function to be called on the next call to
  // `update()` with a positive `dt`.
  void callAfter(absl::Duration delay, Function<void()> f);

  // Register `f` to be called repeatedly after at least `interval` time has
  // passed as seen by `update()`. `interval` must be non-negative.
  //
  // A zero `interval` is a special case that causes a function to be
  // called exactly once per call to `update()` with a positive `dt`. A positive
  // `interval` can cause the function to be called multiple times if the call
  // to `update()` is made with a `dt` larger than `interval`.
  //
  // Note: subsequent timeouts are calculated using the time when a function
  // should have been called, and not when it actually ended up being called. In
  // otherwords, after a cumulative `update()` time of `n * interval`, the
  // function will have been called `n` times.
  void callEvery(absl::Duration interval, Function<void()> f);

  void update(absl::Duration dt);

 private:
  struct Value {
    absl::Time when;
    Function<void()> function;
    absl::Duration repeat;
  };

  struct ValueComparator;

  void consumeStaging(absl::Duration dt);

  absl::Mutex staging_mutex_;
  absl::Time staging_time_ = absl::UnixEpoch();
  std::vector<Value> staging_buffer_;

  absl::Mutex update_mutex_;
  absl::Time update_time_;
  std::vector<Function<void()>> call_every_update_;
  std::vector<Value> update_queue_;
};

}  // namespace y
#endif  // GAMMA_COMMON_FUNCTION_QUEUE_HPP_
