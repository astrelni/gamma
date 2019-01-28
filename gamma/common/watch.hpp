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

#ifndef GAMMA_COMMON_WATCH_HPP_
#define GAMMA_COMMON_WATCH_HPP_

#include <chrono>

#include "absl/time/time.h"

namespace y {

class Watch {
 public:
  // Returns the elapsed duration since the last call to `lap()`.
  //
  // Always returns a non-negative duration. The first time this function is
  // called on an object, it returns the time elapsed since construction.
  absl::Duration lap() {
    Clock::time_point now = Clock::now();
    absl::Duration dt = absl::FromChrono(now - last_);
    last_ = now;
    return dt;
  }

 private:
  using Clock = std::chrono::steady_clock;

  Clock::time_point last_ = Clock::now();
};

}  // namespace y
#endif  // GAMMA_COMMON_WATCH_HPP_
