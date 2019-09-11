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

#include "gamma/common/function_queue.hpp"

#include <algorithm>
#include <random>

#include "gtest/gtest.h"

namespace y {
namespace {

TEST(FunctionQueueTest, BasicCallAfter) {
  FunctionQueue queue;
  bool called = false;
  queue.callAfter(absl::Seconds(1), [&called]() { called = true; });
  EXPECT_FALSE(called);

  queue.update(absl::Seconds(1));
  EXPECT_FALSE(called);

  queue.update(absl::Microseconds(1));
  EXPECT_TRUE(called);
}

TEST(FunctionQueueTest, CallAfterZeroDelay) {
  FunctionQueue queue;
  bool called = false;
  queue.callAfter(absl::ZeroDuration(), [&called]() { called = true; });
  EXPECT_FALSE(called);

  queue.update(absl::Nanoseconds(1));
  EXPECT_TRUE(called);
}

TEST(FunctionQueueTest, CallInCorrectOrder) {
  FunctionQueue queue;

  std::vector<int> input;
  for (int i = 0; i < 100; ++i) {
    input.push_back(i);
  }
  std::random_device device;
  std::mt19937 gen(device());
  std::shuffle(input.begin(), input.end(), gen);

  std::vector<int> output;
  for (int i : input) {
    queue.callAfter(absl::Seconds(i), [&output, i]() { output.push_back(i); });
  }

  queue.update(absl::Seconds(100));

  EXPECT_EQ(100, output.size());
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(i, output[i]);
  }
}

}  // namespace
}  // namespace y
