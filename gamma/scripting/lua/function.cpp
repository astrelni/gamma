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

#include "gamma/common/log.hpp"

namespace y {

LuaFunction::LuaFunction(lua_State* L) : LuaReference() {
  YERR_IF(!lua_isfunction(L, -1));
  reset(L);
}

void LuaFunction::setEnvironment(const LuaTable& env) {
  YERR_IF(!env) << "environment table does not hold a valid reference.";
  lua_State* L = rawState();
  YERR_IF(L != env.rawState()) << "function and environment table were not "
                                  "created from the same lua state.";
  push();
  env.push();
  YERR_IF(lua_setfenv(L, -2) == 0);
  lua_pop(L, 1);
}

}  // namespace y
