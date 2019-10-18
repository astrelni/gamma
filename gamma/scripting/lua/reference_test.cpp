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

#include "gamma/runtime/lua/reference.hpp"

#include "gamma/runtime/lua/test_base.hpp"
#include "gtest/gtest.h"

namespace y {
namespace {

class LuaReferenceTest : public LuaTestBase {};

TEST_F(LuaReferenceTest, Construction) {
  LuaReference r1;
  EXPECT_FALSE(r1);
  EXPECT_EQ(nullptr, r1.rawState());

  lua_newtable(L);
  int stack_size = lua_gettop(L);
  LuaReference r2(L);
  EXPECT_TRUE(r2);
  EXPECT_EQ(stack_size - 1, lua_gettop(L));
}

TEST_F(LuaReferenceTest, Move) {
  lua_newtable(L);
  LuaReference r1(L);
  LuaReference r2 = std::move(r1);
  EXPECT_FALSE(r1);
  EXPECT_EQ(nullptr, r1.rawState());

  LuaReference r3;
  r3 = std::move(r2);
  EXPECT_FALSE(r2);
  EXPECT_EQ(nullptr, r2.rawState());
}

TEST_F(LuaReferenceTest, State) {
  lua_newtable(L);
  LuaReference ref(L);
  EXPECT_EQ(L, ref.rawState());
}

TEST_F(LuaReferenceTest, Push) {
  lua_newtable(L);
  lua_pushvalue(L, -1);
  int stack_size = lua_gettop(L);
  LuaReference table(L);
  EXPECT_EQ(stack_size - 1, lua_gettop(L));
  table.push();
  EXPECT_EQ(stack_size, lua_gettop(L));
  EXPECT_TRUE(lua_equal(L, -1, -2));
}

TEST_F(LuaReferenceTest, Reset) {
  lua_newtable(L);
  lua_newtable(L);
  lua_pushvalue(L, -2);
  lua_pushvalue(L, -2);

  LuaReference ref(L);
  ref.push();
  EXPECT_TRUE(lua_equal(L, -1, -3));
  lua_pop(L, 1);

  int stack_size = lua_gettop(L);
  ref.reset(L);
  EXPECT_EQ(stack_size - 1, lua_gettop(L));

  ref.push();
  EXPECT_TRUE(lua_equal(L, -1, -3));
}

TEST_F(LuaReferenceTest, Clear) {
  lua_newtable(L);
  LuaReference ref(L);
  EXPECT_TRUE(ref);
  ref.clear();
  EXPECT_FALSE(ref);
}

}  // namespace
}  // namespace y
