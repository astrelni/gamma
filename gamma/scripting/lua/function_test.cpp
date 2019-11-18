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

#include "gamma/scripting/lua/function.hpp"

#include "gamma/scripting/lua/table.hpp"
#include "gamma/scripting/lua/test_base.hpp"
#include "gtest/gtest.h"

namespace y {
namespace {

class LuaFunctionTest : public LuaTestBase {};

TEST_F(LuaFunctionTest, Environment) {
  auto noop = [](lua_State*) -> int { return 0; };
  lua_pushcfunction(L, noop);
  LuaFunction f(L);

  LuaTable t = LuaNewTable(L);
  f.setEnvironment(t);

  f.push();
  lua_getfenv(L, -1);
  t.push();
  EXPECT_TRUE(lua_equal(L, -1, -2));
}

}  // namespace
}  // namespace y
