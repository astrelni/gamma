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

#ifndef GAMMA_RUNTIME_LUA_TABLE_HPP_
#define GAMMA_RUNTIME_LUA_TABLE_HPP_

#include "lua.hpp"

namespace y {

// RAII wrapper for a Lua reference to a table.
class LuaTable {
 public:
  // Get the reference from the top of the lua stack.
  explicit LuaTable(lua_State* L);

  LuaTable() = default;
  LuaTable(const LuaTable&) = delete;
  LuaTable(LuaTable&&) noexcept;
  LuaTable& operator=(const LuaTable&) = delete;
  LuaTable& operator=(LuaTable&&) noexcept;
  ~LuaTable();

  // Explicitly copy the table reference.
  static LuaTable MakeCopy(const LuaTable& other);
  // Clear reference to table.
  void clear();
  // Check whether object holds a valid reference to a table.
  explicit operator bool() const;

  // Push table onto lua stack.
  void push() const;
  // Get pointer to lua state owning the reference.
  lua_State* rawState() const;

 private:
  lua_State* state_ = nullptr;
  int ref_ = LUA_NOREF;
};

LuaTable LuaNewTable(lua_State* L);

LuaTable LuaGetGlobalEnvironment(lua_State* L);

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline LuaTable::operator bool() const { return ref_ != LUA_NOREF; }

inline void LuaTable::push() const {
  lua_rawgeti(state_, LUA_REGISTRYINDEX, ref_);
}

inline lua_State* LuaTable::rawState() const { return state_; }

}  // namespace y
#endif  // GAMMA_RUNTIME_LUA_TABLE_HPP_
