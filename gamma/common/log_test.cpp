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

#include <string>

#include "gamma/common/log.hpp"
#include "gtest/gtest.h"

namespace y {
namespace {

struct CustomSink : LogSink {
  explicit CustomSink(std::string* s) : target(s) {}

  void WriteLine(absl::string_view line) override {
    *target = std::string(line);
  }

  std::string* target;
};

TEST(LogTest, WriteToCustomSink) {
  std::string output;
  CustomSink sink(&output);
  SetLogSink(&sink);
  YLOG << "test string";
  EXPECT_FALSE(output.empty());
}

void CheckNonNegative(int n) { YERR_IF(n < 0); }

TEST(LogTest, DieOnError) {
  CheckNonNegative(1);
  EXPECT_DEATH_IF_SUPPORTED(CheckNonNegative(-1), "");
}

}  // namespace
}  // namespace y
