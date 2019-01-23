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

#include "gamma/runtime/lua/table.hpp"

#include "gtest/gtest.h"

namespace y {
namespace {

class LuaTableTest : public ::testing::Test {
 public:
  virtual void SetUp() override {
    L = luaL_newstate();
    luaL_openlibs(L);
  }

  virtual void TearDown() override { lua_close(L); }

  lua_State* L;
};

TEST_F(LuaTableTest, Construction) {
  LuaTable t1;
  EXPECT_FALSE(t1);
  EXPECT_EQ(nullptr, t1.rawState());

  lua_newtable(L);
  LuaTable t2(L);
  EXPECT_TRUE(t2);
}

TEST_F(LuaTableTest, Move) {
  lua_newtable(L);
  LuaTable t1(L);
  LuaTable t2 = std::move(t1);
  EXPECT_FALSE(t1);
  EXPECT_EQ(nullptr, t1.rawState());

  LuaTable t3;
  t3 = std::move(t2);
  EXPECT_FALSE(t2);
  EXPECT_EQ(nullptr, t2.rawState());
}

TEST_F(LuaTableTest, State) {
  lua_newtable(L);
  LuaTable table(L);
  EXPECT_EQ(L, table.rawState());
}

TEST_F(LuaTableTest, Push) {
  lua_newtable(L);
  lua_pushvalue(L, -1);
  int stack_size = lua_gettop(L);
  LuaTable table(L);
  EXPECT_EQ(stack_size - 1, lua_gettop(L));
  table.push();
  EXPECT_TRUE(lua_equal(L, -1, -2));
}

TEST_F(LuaTableTest, Copy) {
  lua_newtable(L);
  LuaTable t1(L);
  LuaTable t2 = LuaTable::MakeCopy(t1);
  EXPECT_TRUE(t2);
  t1.push();
  t2.push();
  EXPECT_TRUE(lua_equal(L, -1, -2));
}

TEST_F(LuaTableTest, Clear) {
  lua_newtable(L);
  LuaTable table(L);
  EXPECT_TRUE(table);
  table.clear();
  EXPECT_FALSE(table);
}

TEST_F(LuaTableTest, GlobalEnv) {
  LuaTable table(LuaGetGlobalEnvironment(L));
  EXPECT_TRUE(table);
  table.push();
  lua_pushstring(L, "_G");
  lua_rawget(L, LUA_GLOBALSINDEX);
  EXPECT_TRUE(lua_equal(L, -1, -2));
}

}  // namespace
}  // namespace y
