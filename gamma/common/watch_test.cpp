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

#include "gamma/common/watch.hpp"

#include "absl/time/clock.h"
#include "gtest/gtest.h"

namespace y {
namespace {

TEST(WatchTest, ConstructAndLap) {
  using Clock = std::chrono::steady_clock;

  for (int i = 1; i < 20; ++i) {
    Clock::time_point before_start = Clock::now();
    Watch watch;
    Clock::time_point after_start = Clock::now();

    absl::SleepFor(absl::Milliseconds(i));

    Clock::time_point before_stop = Clock::now();
    absl::Duration dt = watch.lap();
    Clock::time_point after_stop = Clock::now();

    EXPECT_LE(absl::FromChrono(before_stop - after_start), dt);
    EXPECT_GE(absl::FromChrono(after_stop - before_start), dt);
  }
}

TEST(WatchTest, RepeatedLap) {
  using Clock = std::chrono::steady_clock;

  Clock::time_point before_start = Clock::now();
  Watch watch;
  Clock::time_point after_start = Clock::now();
  for (int i = 1; i < 20; ++i) {
    absl::SleepFor(absl::Milliseconds(i));

    Clock::time_point before_stop = Clock::now();
    absl::Duration dt = watch.lap();
    Clock::time_point after_stop = Clock::now();

    EXPECT_LE(absl::FromChrono(before_stop - after_start), dt);
    EXPECT_GE(absl::FromChrono(after_stop - before_start), dt);

    before_start = before_stop;
    after_start = after_stop;
  }
}

}  // namespace
}  // namespace y
