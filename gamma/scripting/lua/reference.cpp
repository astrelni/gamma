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

namespace y {

LuaReference::LuaReference(lua_State* L)
    : state_(L), ref_(luaL_ref(state_, LUA_REGISTRYINDEX)) {}

LuaReference::LuaReference(LuaReference&& x) noexcept
    : state_(x.state_), ref_(x.ref_) {
  x.state_ = nullptr;
  x.ref_ = LUA_NOREF;
}

LuaReference& LuaReference::operator=(LuaReference&& x) noexcept {
  clear();
  state_ = x.state_;
  ref_ = x.ref_;
  x.state_ = nullptr;
  x.ref_ = LUA_NOREF;
  return *this;
}

LuaReference::~LuaReference() { clear(); }

void LuaReference::reset(lua_State* L) {
  clear();
  state_ = L;
  ref_ = luaL_ref(state_, LUA_REGISTRYINDEX);
}

void LuaReference::clear() {
  if (ref_ != LUA_NOREF) {
    luaL_unref(state_, LUA_REGISTRYINDEX, ref_);
    state_ = nullptr;
    ref_ = LUA_NOREF;
  }
}

}  // namespace y
