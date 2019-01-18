// Copyright (c) 2018 Aleksey Strelnikov
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

#include "gamma/common/function_queue.hpp"

#include "absl/algorithm/container.h"
#include "gamma/common/log.hpp"

namespace y {

struct FunctionQueue::ValueComparator {
  bool operator()(const Value& a, const Value& b) { return a.when > b.when; }
};

void FunctionQueue::callAfter(absl::Duration delay, Function<void()> f) {
  YERR_IF(delay < absl::ZeroDuration());
  absl::MutexLock lock(&staging_mutex_);
  staging_buffer_.push_back(
      Value({staging_time_ + delay, std::move(f), absl::InfiniteDuration()}));
}

void FunctionQueue::callEvery(absl::Duration interval, Function<void()> f) {
  YERR_IF(interval < absl::ZeroDuration());
  absl::MutexLock lock(&staging_mutex_);
  staging_buffer_.push_back(
      Value({staging_time_ + interval, std::move(f), interval}));
}

void FunctionQueue::consumeStaging(absl::Duration dt) {
  absl::MutexLock lock(&staging_mutex_);

  staging_time_ += dt;
  update_time_ = staging_time_;

  for (Value& value : staging_buffer_) {
    if (value.repeat == absl::ZeroDuration()) {
      call_every_update_.push_back(std::move(value.function));
    } else {
      queue_.push_back(std::move(value));
      absl::c_push_heap(queue_, ValueComparator());
    }
  }
  staging_buffer_.clear();
}

void FunctionQueue::update(absl::Duration dt) {
  if (dt <= absl::ZeroDuration()) return;

  consumeStaging(dt);

  for (Function<void()>& f : call_every_update_) f();

  while (!queue_.empty() && queue_.front().when < update_time_) {
    absl::c_pop_heap(queue_, ValueComparator());
    Value& value = queue_.back();
    value.function();
    if (value.repeat == absl::InfiniteDuration()) {
      queue_.pop_back();
    } else {
      value.when += value.repeat;
      absl::c_push_heap(queue_, ValueComparator());
    }
  }
}

}  // namespace y
