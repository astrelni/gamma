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

#ifndef GAMMA_RUNTIME_LUA_INVOKE_HPP_
#define GAMMA_RUNTIME_LUA_INVOKE_HPP_

#include <utility>

#include "gamma/runtime/lua/call.hpp"
#include "gamma/runtime/lua/function.hpp"
#include "gamma/runtime/lua/stack.hpp"

namespace y {

void LuaInvoke(const LuaFunction& func);

template <typename... Args>
void LuaInvoke(const LuaFunction& func, Args&&... args);

template <typename R>
R LuaInvoke(const LuaFunction& func);

template <typename R, typename... Args>
R LuaInvoke(const LuaFunction& func, Args&&... args);

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline void LuaInvoke(const LuaFunction& func) {
  func.push();
  LuaPCall(func.rawState(), 0);
}

template <typename... Args>
void LuaInvoke(const LuaFunction& func, Args&&... args) {
  func.push();
  ::y::LuaPush(func.rawState(), std::forward<Args>(args)...);
  LuaPCall(func.rawState(), sizeof...(args));
}

template <typename R>
R LuaInvoke(const LuaFunction& func) {
  func.push();
  return LuaPCall<R>(func.rawState(), 0);
}

template <typename R, typename... Args>
R LuaInvoke(const LuaFunction& func, Args&&... args) {
  func.push();
  ::y::LuaPush(func.rawState(), std::forward<Args>(args)...);
  return LuaPCall<R>(func.rawState(), sizeof...(args));
}

}  // namespace y
#endif  // GAMMA_RUNTIME_LUA_INVOKE_HPP_
