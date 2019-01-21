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

#ifndef GAMMA_RUNTIME_LUA_STATE_HPP_
#define GAMMA_RUNTIME_LUA_STATE_HPP_

#include "lua.hpp"

namespace y {

// RAII wrapper over lua_State
class LuaState {
 public:
  LuaState() = default;
  LuaState(LuaState&&) noexcept;
  LuaState& operator=(LuaState&&) noexcept;
  ~LuaState();

  LuaState(const LuaState&) = delete;
  LuaState& operator=(const LuaState&) = delete;

  lua_State* get() const;

  explicit operator bool() const;

  static LuaState Open();

 private:
  LuaState(lua_State* state) : state_(state) {}
  void clear();

  lua_State* state_ = nullptr;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline LuaState::LuaState(LuaState&& x) noexcept : state_(x.state_) {
  x.state_ = nullptr;
}

inline LuaState& LuaState::operator=(LuaState&& x) noexcept {
  clear();
  state_ = x.state_;
  x.state_ = nullptr;
  return *this;
}

inline LuaState::~LuaState() { clear(); }

inline lua_State* LuaState::get() const { return state_; }

inline LuaState::operator bool() const { return state_ != nullptr; }

inline void LuaState::clear() {
  if (state_ != nullptr) lua_close(state_);
}

}  // namespace y
#endif  // GAMMA_RUNTIME_LUA_HELPERS_STATE_HPP_
