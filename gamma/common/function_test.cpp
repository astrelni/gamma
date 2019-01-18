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

#include "gamma/common/function.hpp"

#include "gtest/gtest.h"

namespace y {
namespace {

void SetTrue(bool* b) { *b = true; }

TEST(FunctionTest, BasicFunctionPointer) {
  Function<void(bool*)> func = &SetTrue;
  EXPECT_TRUE(func);

  bool called = false;
  func(&called);
  EXPECT_TRUE(called);

  func.clear();
  EXPECT_FALSE(func);

  func = &SetTrue;
  EXPECT_TRUE(func);

  called = false;
  func(&called);
  EXPECT_TRUE(called);
}

TEST(FunctionTest, BasicFunctionReference) {
  using FuncRef = void (&)(bool*);

  Function<void(bool*)> func = static_cast<FuncRef>(SetTrue);
  EXPECT_TRUE(func);

  bool called = false;
  func(&called);
  EXPECT_TRUE(called);

  func.clear();
  EXPECT_FALSE(func);

  func = static_cast<FuncRef>(SetTrue);
  EXPECT_TRUE(func);

  called = false;
  func(&called);
  EXPECT_TRUE(called);
}

TEST(FunctionTest, BasicUserType) {
  struct SetTrueType {
    void operator()(bool* b) const { *b = true; }
  } t;

  Function<void(bool*)> func = t;
  EXPECT_TRUE(func);

  bool called = false;
  func(&called);
  EXPECT_TRUE(called);

  func.clear();
  EXPECT_FALSE(func);

  func = t;
  EXPECT_TRUE(func);

  called = false;
  func(&called);
  EXPECT_TRUE(called);
}

TEST(FunctionTest, BasicLambda) {
  Function<void(bool*)> func([](bool* b) { *b = true; });
  EXPECT_TRUE(func);

  bool called = false;
  func(&called);
  EXPECT_TRUE(called);

  func.clear();
  EXPECT_FALSE(func);

  func = [](bool* b) { *b = true; };
  EXPECT_TRUE(func);

  called = false;
  func(&called);
  EXPECT_TRUE(called);
}

TEST(FunctionTest, StoresState) {
  int x = 0;
  Function<int()> func([&x]() { return x; });
  x = 42;
  EXPECT_EQ(x, func());
}

TEST(FunctionTest, ErasedSizeAndAlignment) {
  struct GetSum {
    int* a;
    int* b;

    int operator()() const { return *a + *b; }
  };

  int x = 5;
  int y = 3;
  GetSum summer = {&x, &y};

  Function<int()> func = summer;
  EXPECT_EQ(x + y, func());
}

TEST(FunctionTest, MutatesState) {
  struct Counter {
    int n = 0;

    int operator()() { return n++; }
  } counter;
  Function<int()> func(counter);

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(i, func());
  }
}

TEST(FunctionTest, Nullptr) {
  Function<void()> func = []() {};
  EXPECT_TRUE(func);
  func = nullptr;
  EXPECT_FALSE(func);

  Function<char*(int, float, double)> constructed_from_null = nullptr;
  EXPECT_FALSE(constructed_from_null);
}

TEST(FunctionTest, CopyAndMoveErasedObject) {
  class CheckCopyMove {
   private:
    bool* copy_flag;
    bool* move_flag;

   public:
    CheckCopyMove() = default;
    CheckCopyMove(bool* copy, bool* move) : copy_flag(copy), move_flag(move) {}
    CheckCopyMove(const CheckCopyMove& other)
        : copy_flag(other.copy_flag), move_flag(other.move_flag) {
      if (copy_flag) *copy_flag = true;
    }
    CheckCopyMove(CheckCopyMove&& other)
        : copy_flag(other.copy_flag), move_flag(other.move_flag) {
      if (move_flag) *move_flag = true;
      other.copy_flag = nullptr;
      other.move_flag = nullptr;
    }

    void operator()() {}
  };

  bool copied = false;
  bool moved = false;
  {
    CheckCopyMove func_object(&copied, &moved);
    Function<void()> func = std::move(func_object);
    func();
  }
  EXPECT_FALSE(copied);
  EXPECT_TRUE(moved);

  copied = false;
  moved = false;
  {
    CheckCopyMove func_object(&copied, &moved);
    Function<void()> func;
    func = std::move(func_object);
    func();
  }
  EXPECT_FALSE(copied);
  EXPECT_TRUE(moved);

  copied = false;
  moved = false;
  {
    CheckCopyMove func_object(&copied, &moved);
    Function<void()> func = func_object;
    func();
  }
  EXPECT_TRUE(copied);
  EXPECT_FALSE(moved);

  copied = false;
  moved = false;
  {
    CheckCopyMove func_object(&copied, &moved);
    Function<void()> func;
    func = func_object;
    func();
  }
  EXPECT_TRUE(copied);
  EXPECT_FALSE(moved);
}

TEST(FunctionTest, DestroysErasedObject) {
  struct Destroy {
    bool* destroy_flag;

    ~Destroy() { *destroy_flag = true; }

    void operator()() const {}
  };

  bool destroyed = false;
  Destroy func_object = {&destroyed};
  {
    Function<void()> func = func_object;
    EXPECT_FALSE(destroyed);
  }
  EXPECT_TRUE(destroyed);

  destroyed = false;
  {
    Function<void()> func;
    func = func_object;
    EXPECT_FALSE(destroyed);
  }
  EXPECT_TRUE(destroyed);
}

TEST(FunctionTest, NonCopyableTypesInSignature) {
  struct NoCopy {
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete;
    NoCopy(NoCopy&&) = default;
    NoCopy& operator=(const NoCopy&) = delete;
    NoCopy& operator=(NoCopy&&) = default;

    int get() { return 42; }
  };

  Function<int(NoCopy)> func = [](NoCopy nc) -> int { return nc.get(); };

  NoCopy no_copy;
  EXPECT_EQ(42, func(std::move(no_copy)));

  int num = 0;
  Function<NoCopy()> func2 = [&num]() -> NoCopy {
    NoCopy nc;
    num = nc.get();
    return nc;
  };
  NoCopy nc = func2();
  (void)nc;
  EXPECT_EQ(42, num);
}

TEST(FunctionTest, LValueReferences) {
  Function<void(int&)> func = [](int& num) { num = 42; };
  int x = 0;
  func(x);
  EXPECT_EQ(42, x);

  int y = 0;
  Function<int&()> func2 = [&y]() -> int& { return y; };
  func2() = 42;
  EXPECT_EQ(42, y);
}

}  // namespace
}  // namespace y
