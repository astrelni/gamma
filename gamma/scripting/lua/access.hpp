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

#ifndef GAMMA_SCRIPTING_LUA_ACCESS_HPP_
#define GAMMA_SCRIPTING_LUA_ACCESS_HPP_

#include <type_traits>
#include <utility>

#include "absl/strings/string_view.h"
#include "gamma/scripting/lua/stack.hpp"
#include "lua.hpp"

namespace y {

// Set a global variable to a particular value. Leaves the lua stack unchanged.
template <typename T>
void LuaRawSet(lua_State* L, absl::string_view key, T&& value);

// Set the field of a table to a particular value. Leaves the lua stack
// unchanged.
template <typename T>
void LuaRawSet(LuaTable* t, absl::string_view key, T&& value);

// Get the value of a global variable. Errors out if the value is not
// convertible to T. Leaves the lua stack unchanged.
template <typename T>
T LuaRawGet(lua_State* L, absl::string_view key);

// Get the value of a table field. Errors out if the value is not converible to
// T. Leaves the lua stack unchanged.
template <typename T>
T LuaRawGet(LuaTable* t, absl::string_view key);

}  // namespace y

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

namespace y_internal {

template <typename T, typename std::enable_if<
                          !std::is_same<typename std::decay<T>::type,
                                        const char*>::value>::type* = nullptr>
T LuaRawGetImpl(lua_State* L, absl::string_view key) {
  y::LuaPush(L, key);
  lua_rawget(L, LUA_GLOBALSINDEX);
  T value = ::y::LuaGet<T>(L, -1);
  lua_pop(L, 1);
  return std::forward<T>(value);
}

template <typename T, typename std::enable_if<
                          !std::is_same<typename std::decay<T>::type,
                                        const char*>::value>::type* = nullptr>
T LuaRawGetImpl(y::LuaTable* t, absl::string_view key) {
  lua_State* L = t->State();
  y::LuaPush(L, *t, key);
  lua_rawget(L, -2);
  T value = ::y::LuaGet<T>(L, -1);
  lua_pop(L, 2);
  return std::forward<T>(value);
}

template <typename T, typename std::enable_if<
                          std::is_same<typename std::decay<T>::type,
                                       const char*>::value>::type* = nullptr>
T LuaRawGetImpl(lua_State* L, absl::string_view key) = delete;

template <typename T, typename std::enable_if<
                          std::is_same<typename std::decay<T>::type,
                                       const char*>::value>::type* = nullptr>
T LuaRawGetImpl(y::LuaTable* t, absl::string_view key) = delete;

}  // namespace y_internal

namespace y {

template <typename T>
void LuaRawSet(lua_State* L, absl::string_view key, T&& value) {
  ::y::LuaPush(L, key, std::forward<T>(value));
  lua_rawset(L, LUA_GLOBALSINDEX);
}

template <typename T>
void LuaRawSet(LuaTable* t, absl::string_view key, T&& value) {
  lua_State* L = t->State();
  ::y::LuaPush(L, *t, key, std::forward<T>(value));
  lua_rawset(L, -3);
  lua_pop(L, 1);
}

template <typename T>
T LuaRawGet(lua_State* L, absl::string_view key) {
  return std::forward<T>(::y_internal::LuaRawGetImpl<T>(L, key));
}

template <typename T>
T LuaRawGet(LuaTable* t, absl::string_view key) {
  return std::forward<T>(::y_internal::LuaRawGetImpl<T>(t, key));
}

}  // namespace y
#endif  // GAMMA_SCRIPTING_LUA_ACCESS_HPP_
