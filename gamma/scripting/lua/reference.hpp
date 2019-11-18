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

#ifndef GAMMA_SCRIPTING_LUA_REFERENCE_HPP_
#define GAMMA_SCRIPTING_LUA_REFERENCE_HPP_

#include "lua.hpp"

namespace y {

// RAII wrapper for a Lua reference.
class LuaReference {
 public:
  // Pop and get a reference to the value at the top of the lua stack.
  explicit LuaReference(lua_State* L);

  LuaReference() = default;
  LuaReference(const LuaReference&) = delete;
  LuaReference(LuaReference&&) noexcept;
  LuaReference& operator=(const LuaReference&) = delete;
  LuaReference& operator=(LuaReference&&) noexcept;
  ~LuaReference();

  // Reset to a reference to the value at the top of the stack, which is popped.
  void reset(lua_State* L);
  // Clear the reference.
  void clear();
  // Check whether object holds a valid reference.
  explicit operator bool() const;

  // Push Reference onto lua stack.
  void push() const;
  // Get pointer to lua state owning the reference.
  lua_State* rawState() const;

 private:
  lua_State* state_ = nullptr;
  int ref_ = LUA_NOREF;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline LuaReference::operator bool() const { return ref_ != LUA_NOREF; }

inline void LuaReference::push() const {
  lua_rawgeti(state_, LUA_REGISTRYINDEX, ref_);
}

inline lua_State* LuaReference::rawState() const { return state_; }

}  // namespace y
#endif  // GAMMA_SCRIPTING_LUA_REFERENCE_HPP_
