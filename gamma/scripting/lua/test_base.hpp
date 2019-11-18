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

#ifndef GAMMA_SCRIPTING_LUA_TESTING_BASE_HPP_
#define GAMMA_SCRIPTING_LUA_TESTING_BASE_HPP_

#include "gtest/gtest.h"
#include "lua.hpp"

namespace y {

class LuaTestBase : public ::testing::Test {
 protected:
  virtual void SetUp() override {
    L = luaL_newstate();
    luaL_openlibs(L);
  }

  virtual void TearDown() override { lua_close(L); }

  lua_State* L;
};

}  // namespace y

#endif  // GAMMA_SCRIPTING_LUA_TESTING_BASE_HPP_
