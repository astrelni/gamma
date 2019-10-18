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

#ifndef GAMMA_RUNTIME_LUA_CALL_HPP_
#define GAMMA_RUNTIME_LUA_CALL_HPP_

#include <utility>

#include "gamma/common/log.hpp"
#include "gamma/runtime/lua/stack.hpp"
#include "lua.hpp"

namespace y {

// Call the function at the top of the lua stack, logging any errors. Pops the
// function off the top of the stack. In the case of returning a value, does not
// leave that value on the stack.
void LuaPCall(lua_State* L, int nargs);
template <typename R>
R LuaPCall(lua_State* L, int nargs);

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline void LuaPCall(lua_State* L, int nargs) {
  if (lua_pcall(L, nargs, 0, 0)) YERR_RAW << lua_tostring(L, -1);
}

template <typename R>
R LuaPCall(lua_State* L, int nargs) {
  if (lua_pcall(L, nargs, 1, 0)) YERR_RAW << lua_tostring(L, -1);
  R r = ::y::LuaGet<R>(L, 1);
  lua_pop(L, 1);
  return std::forward<R>(r);
}

}  // namespace y
#endif  // GAMMA_RUNTIME_LUA_CALL_HPP_
