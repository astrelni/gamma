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

#ifndef GAMMA_SCRIPTING_LUA_TABLE_HPP_
#define GAMMA_SCRIPTING_LUA_TABLE_HPP_

#include "gamma/scripting/lua/reference.hpp"

namespace y {

// RAII wrapper for a Lua reference to a table.
class LuaTable : public LuaReference {
 public:
  explicit LuaTable(lua_State* L);

  LuaTable() = default;
};

LuaTable LuaNewTable(lua_State* L);

LuaTable LuaGetGlobalEnvironment(lua_State* L);

}  // namespace y
#endif  // GAMMA_SCRIPTING_LUA_TABLE_HPP_
