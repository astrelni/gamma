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

#include <algorithm>

#include "absl/algorithm/container.h"

namespace y {

struct FunctionQueue::ValueComparator {
  bool operator()(const Value& a, const Value& b) { return a.when > b.when; }
};

void FunctionQueue::runAfter(absl::Duration delay, Function<void()> f) {
  absl::MutexLock lock(&staging_mutex_);
  using std::max;
  staging_buffer_.push_back(Value({now_ + max(delay, absl::ZeroDuration()),
                                   std::move(f), absl::InfiniteDuration()}));
}

void FunctionQueue::runEvery(absl::Duration interval, Function<void()> f) {
  absl::MutexLock lock(&staging_mutex_);
  using std::max;
  staging_buffer_.push_back(Value(
      {now_ + max(interval, absl::ZeroDuration()), std::move(f), interval}));
}

void FunctionQueue::consumeStagingBuffer() {
  absl::MutexLock lock(&staging_mutex_);
  for (Value& value : staging_buffer_) {
    if (value.repeat == absl::ZeroDuration()) {
      run_every_update_.push_back(std::move(value.function));
    } else {
      queue_.push_back(std::move(value));
      absl::c_push_heap(queue_, ValueComparator());
    }
  }
  staging_buffer_.clear();
}

void FunctionQueue::update(absl::Duration dt) {
  if (dt <= absl::ZeroDuration()) return;

  consumeStagingBuffer();

  for (Function<void()>& f : run_every_update_) f();

  now_ += dt;
  while (!queue_.empty() && queue_.front().when < now_) {
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
