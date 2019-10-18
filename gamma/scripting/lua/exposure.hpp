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

#ifndef GAMMA_RUNTIME_LUA_EXPOSURE_HPP_
#define GAMMA_RUNTIME_LUA_EXPOSURE_HPP_

#include <initializer_list>
#include <string>
#include <type_traits>

#include "absl/strings/string_view.h"
#include "engine/lua/access.hpp"
#include "engine/lua/stack.hpp"
#include "engine/lua/table.hpp"
#include "third_party/luajit/lua.hpp"

namespace y {

using LuaExposureList =
    std::initializer_list<std::pair<absl::string_view, lua_CFunction>>;

void LuaExposeFunctions(lua_State* L, LuaExposureList free_functions);
void LuaExposeFunctions(LuaTable* t, LuaExposureList free_functions);

template <typename T,
          typename std::enable_if<!std::is_pointer<T>::value &&
                                  !std::is_reference<T>::value &&
                                  !std::is_const<T>::value>::type* = nullptr>
void LuaExposeType(lua_State* L, LuaExposureList nonstatic_methods);

template <typename T,
          typename std::enable_if<!std::is_pointer<T>::value &&
                                  !std::is_reference<T>::value &&
                                  !std::is_const<T>::value>::type* = nullptr>
void LuaExposeType(LuaTable* t, LuaExposureList nonstatic_methods,
                   LuaExposureList static_methods);

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

template <typename T,
          typename std::enable_if<!std::is_pointer<T>::value &&
                                  !std::is_reference<T>::value &&
                                  !std::is_const<T>::value>::type* = nullptr>
void LuaExposeType(lua_State* L, LuaExposureList nonstatic_methods) {
  y_internal::LuaPushMetatable<T*>(L);
  for (const auto& kv : nonstatic_methods) {
    LuaPush(L, kv.first, kv.second);
    lua_rawset(L, -3);
  }
  lua_pop(L, 1);

  y_internal::LuaPushMetatable<const T*>(L);
  for (const auto& kv : nonstatic_methods) {
    LuaPush(L, kv.first, kv.second);
    lua_rawset(L, -3);
  }
  lua_pop(L, 1);
}  // namespace y

template <typename T,
          typename std::enable_if<!std::is_pointer<T>::value &&
                                  !std::is_reference<T>::value &&
                                  !std::is_const<T>::value>::type* = nullptr>
void LuaExposeType(LuaTable* t, LuaExposureList nonstatic_methods,
                   LuaExposureList static_methods) {
  t->Push();
  lua_State* L = t->State();

  for (const auto& kv : static_methods) {
    LuaPush(L, kv.first, kv.second);
    lua_rawset(L, -3);
  }

  y_internal::LuaPushMetatable<T*>(L);
  lua_setmetatable(L, -2);
  lua_pop(L, 1);

  y_internal::LuaPushMetatable<T*>(L);
  for (const auto& kv : nonstatic_methods) {
    LuaPush(L, kv.first, kv.second);
    lua_rawset(L, -3);
  }
  lua_pop(L, 1);

  y_internal::LuaPushMetatable<const T*>(L);
  for (const auto& kv : nonstatic_methods) {
    LuaPush(L, kv.first, kv.second);
    lua_rawset(L, -3);
  }
  lua_pop(L, 1);
}

}  // namespace y
#endif  // GAMMA_RUNTIME_LUA_EXPOSURE_HPP_
