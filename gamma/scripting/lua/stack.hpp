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

#ifndef GAMMA_SCRIPTING_LUA_STACK_HPP_
#define GAMMA_SCRIPTING_LUA_STACK_HPP_

#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include "absl/base/optimization.h"
#include "absl/meta/type_traits.h"
#include "absl/strings/string_view.h"
#include "gamma/common/log.hpp"
#include "gamma/scripting/lua/function.hpp"
#include "gamma/scripting/lua/table.hpp"
#include "lua.hpp"

namespace y {

// Push values onto the stack.
template <typename... Values>
void LuaPush(lua_State* L, Values&&... values);

// Construct a value on the stack in-place.
template <typename T, typename... Args>
T* LuaEmplace(lua_State* L, Args&&... args);

// Get the object at a particular index. Errors out if the type at the given
// index is not T. Does not modify the stack.
template <typename T>
T LuaGet(lua_State* L, int index);

}  // namespace y

// Define specializations for key getters to get a type exposed in Lua. These
// specializations are for use in place of typeid(T).name() which can create
// really long mangled names that are also different depending on compiler.
#define GAMMA_CREATE_LUA_METATABLE_KEYS(type)               \
  namespace y_internal {                                    \
  template <>                                               \
  struct LuaTypeMetatableKey<type> {                        \
    static constexpr char* Get() { return #type; }          \
  };                                                        \
  template <>                                               \
  struct LuaTypeMetatableKey<type*> {                       \
    static constexpr char* Get() { return #type "*"; }      \
  };                                                        \
  template <>                                               \
  struct LuaTypeMetatableKey<const type*> {                 \
    static constexpr char* Get() { return "K " #type "*"; } \
  };                                                        \
  }

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

namespace y_internal {

template <typename T>
int LuaDestructor(lua_State* L) {
  static_cast<T*>(lua_touserdata(L, 1))->~T();
  return 0;
}

template <typename T>
struct LuaTypeMetatableKey;

template <typename T, absl::enable_if_t<std::is_pointer<T>::value>* = nullptr>
void LuaPushMetatable(lua_State* L) {
  lua_pushstring(L, ::y_internal::LuaTypeMetatableKey<T>::Get());
  lua_rawget(L, LUA_REGISTRYINDEX);  // find registry.typename
  if (ABSL_PREDICT_TRUE(!lua_isnil(L, -1))) return;

  lua_pop(L, 1);    // pop nil
  lua_newtable(L);  // push what will become the metatable
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);  // (c)pointer_table.__index = (c)pointer_table
  lua_pushstring(L, ::y_internal::LuaTypeMetatableKey<T>::Get());
  lua_pushvalue(L, -2);                // push copy reference to metatable
  lua_settable(L, LUA_REGISTRYINDEX);  // registry.typename = metatable
}

template <typename T, absl::enable_if_t<!std::is_pointer<T>::value>* = nullptr>
void LuaPushMetatable(lua_State* L) {
  lua_pushstring(L, ::y_internal::LuaTypeMetatableKey<T>::Get());
  lua_rawget(L, LUA_REGISTRYINDEX);  // find registry.typename
  if (ABSL_PREDICT_TRUE(!lua_isnil(L, -1))) return;

  lua_pop(L, 1);    // pop nil
  lua_newtable(L);  // push what will become the metatable
  lua_pushstring(L, "__index");
  ::y_internal::LuaPushMetatable<T*>(L);
  lua_settable(L, -3);  // non_pointer_table.__index = pointer_table
  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, &::y_internal::LuaDestructor<T>);
  lua_settable(L, -3);  // non_pointer_table.__gc = destructor
  lua_pushstring(L, ::y_internal::LuaTypeMetatableKey<T>::Get());
  lua_pushvalue(L, -2);                // push copy reference to metatable
  lua_settable(L, LUA_REGISTRYINDEX);  // registry.typename = metatable
}

template <
    typename T,
    absl::enable_if_t<std::is_arithmetic<absl::decay_t<T>>::value &&
                      !std::is_same<absl::decay_t<T>, bool>::value &&
                      !std::is_same<absl::decay_t<T>, char>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  lua_pushnumber(L, static_cast<lua_Number>(t));
}

template <
    typename T,
    absl::enable_if_t<std::is_same<absl::decay_t<T>, bool>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  lua_pushboolean(L, static_cast<int>(t));
}

template <
    typename T,
    absl::enable_if_t<std::is_same<absl::decay_t<T>, char>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  lua_pushlstring(L, &t, 1);
}

template <typename T, absl::enable_if_t<std::is_same<
                          absl::decay_t<T>, const char*>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  lua_pushstring(L, t);
}

template <typename T, absl::enable_if_t<std::is_same<
                          absl::decay_t<T>, std::string>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  lua_pushlstring(L, t.c_str(), t.length());
}

template <typename T,
          absl::enable_if_t<std::is_same<absl::decay_t<T>,
                                         absl::string_view>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  lua_pushlstring(L, t.data(), t.length());
}

template <typename T,
          absl::enable_if_t<
              !std::is_arithmetic<absl::decay_t<T>>::value &&
              std::is_pointer<absl::decay_t<T>>::value &&
              !std::is_same<absl::decay_t<T>, const char*>::value &&
              !std::is_same<absl::decay_t<T>, lua_CFunction>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaTable*>::value &&
              !std::is_same<absl::decay_t<T>, const ::y::LuaTable*>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaFunction*>::value &&
              !std::is_same<absl::decay_t<T>,
                            const ::y::LuaFunction*>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  if (t == nullptr) {
    lua_pushnil(L);
    return;
  }

  using Base = absl::decay_t<absl::remove_pointer_t<T>>;
  auto x = static_cast<Base**>(lua_newuserdata(L, sizeof(Base*)));
  *x = const_cast<Base*>(t);
  ::y_internal::LuaPushMetatable<T>(L);
  lua_setmetatable(L, -2);
}

template <typename T, absl::enable_if_t<std::is_same<
                          absl::decay_t<T>, lua_CFunction>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  if (t != nullptr) {
    lua_pushcfunction(L, t);
  } else {
    lua_pushnil(L);
  }
}

template <typename T,
          absl::enable_if_t<
              std::is_same<absl::decay_t<T>, ::y::LuaTable*>::value ||
              std::is_same<absl::decay_t<T>, const ::y::LuaTable*>::value ||
              std::is_same<absl::decay_t<T>, ::y::LuaFunction*>::value ||
              std::is_same<absl::decay_t<T>, const ::y::LuaFunction*>::value>* =
              nullptr>
void LuaPush(lua_State* L, T&& t) {
  if (t != nullptr && *t) {
    YERR_IF(L != t->rawState());
    t->push();
  } else {
    lua_pushnil(L);
  }
}

template <
    typename T,
    absl::enable_if_t<
        std::is_same<absl::decay_t<T>, ::y::LuaTable>::value ||
        std::is_same<absl::decay_t<T>, ::y::LuaFunction>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  if (t) {
    YERR_IF(L != t.rawState());
    t.push();
  } else {
    lua_pushnil(L);
  }
}

template <typename T,
          absl::enable_if_t<
              std::is_same<absl::decay_t<T>,
                           std::reference_wrapper<::y::LuaTable>>::value ||
              std::is_same<absl::decay_t<T>,
                           std::reference_wrapper<::y::LuaFunction>>::value>* =
              nullptr>
void LuaPush(lua_State* L, T&& t) {
  if (t.get()) {
    YERR_IF(L != t.get().rawState());
    t.push();
  } else {
    lua_pushnil(L);
  }
}

template <typename T,
          absl::enable_if_t<
              !std::is_arithmetic<absl::decay_t<T>>::value &&
              !std::is_pointer<absl::decay_t<T>>::value &&
              !std::is_same<absl::decay_t<T>, std::string>::value &&
              !std::is_same<absl::decay_t<T>, absl::string_view>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaTable>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaFunction>::value &&
              !std::is_same<absl::decay_t<T>,
                            std::reference_wrapper<::y::LuaTable>>::value &&
              !std::is_same<absl::decay_t<T>,
                            std::reference_wrapper<::y::LuaFunction>>::value &&
              std::is_lvalue_reference<T>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  using Base = absl::decay_t<T>;
  auto x = static_cast<Base**>(lua_newuserdata(L, sizeof(Base*)));
  *x = const_cast<Base*>(&t);
  ::y_internal::LuaPushMetatable<decltype(&t)>(L);
  lua_setmetatable(L, -2);
}

template <typename T,
          absl::enable_if_t<
              !std::is_arithmetic<absl::decay_t<T>>::value &&
              !std::is_pointer<absl::decay_t<T>>::value &&
              !std::is_same<absl::decay_t<T>, std::string>::value &&
              !std::is_same<absl::decay_t<T>, absl::string_view>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaTable>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaFunction>::value &&
              !std::is_same<absl::decay_t<T>,
                            std::reference_wrapper<::y::LuaTable>>::value &&
              !std::is_same<absl::decay_t<T>,
                            std::reference_wrapper<::y::LuaFunction>>::value &&
              !std::is_lvalue_reference<T>::value>* = nullptr>
void LuaPush(lua_State* L, T&& t) {
  using Base = absl::decay_t<T>;
  ::new (lua_newuserdata(L, sizeof(Base))) Base(std::forward<T>(t));
  ::y_internal::LuaPushMetatable<Base>(L);
  lua_setmetatable(L, -2);
}

template <typename T>
struct IsNonConstLValueReference
    : absl::conjunction<
          std::is_lvalue_reference<T>,
          absl::negation<std::is_const<absl::remove_reference_t<T>>>> {};

}  // namespace y_internal

namespace y {

template <typename T,
          absl::enable_if_t<
              (std::is_arithmetic<T>::value || std::is_enum<T>::value) &&
              !std::is_same<T, bool>::value && !std::is_same<T, char>::value>* =
              nullptr>
T LuaGet(lua_State* L, int i) {
  return static_cast<T>(luaL_checknumber(L, i));
}

template <typename T,
          absl::enable_if_t<std::is_same<T, bool>::value>* = nullptr>
T LuaGet(lua_State* L, int i) {
  return lua_toboolean(L, i) != 0;
}

template <typename T,
          absl::enable_if_t<std::is_same<T, char>::value>* = nullptr>
T LuaGet(lua_State* L, int i) {
  if (lua_type(L, i) == LUA_TSTRING) {
    const char* p = lua_tostring(L, i);
    return p == nullptr ? 0 : *p;
  }
  return static_cast<char>(luaL_checknumber(L, i));
}

template <typename T, absl::enable_if_t<
                          !::y_internal::IsNonConstLValueReference<T>::value &&
                          std::is_constructible<T, const char*>::value &&
                          !std::is_same<T, bool>::value>* = nullptr>
absl::decay_t<T> LuaGet(lua_State* L, int i) {
  const char* p = lua_tostring(L, i);
  if (p == nullptr) luaL_error(L, "Bad argument %d expected string.", i);
  return absl::decay_t<T>(p);  // decay to support const string& for example
}

template <typename T,
          absl::enable_if_t<
              !::y_internal::IsNonConstLValueReference<T>::value &&
              std::is_same<absl::decay_t<T>, y::LuaTable>::value>* = nullptr>
y::LuaTable LuaGet(lua_State* L, int i) {
  luaL_argcheck(L, lua_istable(L, i) != 0, i, "invalid type, expected table");
  lua_pushvalue(L, i);
  return y::LuaTable(L);
}

template <typename T,
          absl::enable_if_t<
              !::y_internal::IsNonConstLValueReference<T>::value &&
              std::is_same<absl::decay_t<T>, y::LuaFunction>::value>* = nullptr>
y::LuaFunction LuaGet(lua_State* L, int i) {
  luaL_argcheck(L, lua_isfunction(L, i) != 0, i,
                "invalid type, expected function");
  lua_pushvalue(L, i);
  return y::LuaFunction(L);
}

template <typename T,
          absl::enable_if_t<
              !std::is_arithmetic<T>::value &&
              !std::is_constructible<T, const char*>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaTable>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaFunction>::value &&
              std::is_pointer<T>::value &&
              std::is_const<absl::remove_pointer_t<T>>::value>* = nullptr>
T LuaGet(lua_State* L, int i) {
  using Base = absl::decay_t<absl::remove_pointer_t<T>>;
  luaL_argcheck(L, lua_getmetatable(L, i) != 0, i, "type has no metadata");
  ::y_internal::LuaPushMetatable<Base>(L);
  if (!lua_equal(L, -1, -2)) {
    lua_pop(L, 1);
    ::y_internal::LuaPushMetatable<Base*>(L);
    if (!lua_equal(L, -1, -2)) {
      lua_pop(L, 1);
      ::y_internal::LuaPushMetatable<const Base*>(L);
      luaL_argcheck(L, lua_equal(L, -1, -2) == 1, i, "invalid type detected");
    }
    lua_pop(L, 2);
    return std::forward<T>(*static_cast<Base**>(lua_touserdata(L, i)));
  }
  lua_pop(L, 2);
  return std::forward<T>(static_cast<Base*>(lua_touserdata(L, i)));
}

template <typename T,
          absl::enable_if_t<
              !std::is_arithmetic<T>::value &&
              !std::is_constructible<T, const char*>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaTable>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaFunction>::value &&
              std::is_pointer<T>::value &&
              !std::is_const<absl::remove_pointer_t<T>>::value &&
              !std::is_same<T, void*>::value>* = nullptr>
T LuaGet(lua_State* L, int i) {
  using Base = absl::decay_t<absl::remove_pointer_t<T>>;
  luaL_argcheck(L, lua_getmetatable(L, i) != 0, i, "type has no metadata");
  ::y_internal::LuaPushMetatable<Base>(L);
  if (!lua_equal(L, -1, -2)) {
    lua_pop(L, 1);
    ::y_internal::LuaPushMetatable<Base*>(L);
    if (!lua_equal(L, -1, -2)) {
      lua_pop(L, 2);
      luaL_argerror(L, i,
                    "invalid type, possibly expected non-const and got const");
    }
    lua_pop(L, 2);
    return std::forward<T>(*static_cast<Base**>(lua_touserdata(L, i)));
  }
  lua_pop(L, 2);
  return std::forward<T>(static_cast<Base*>(lua_touserdata(L, i)));
}

template <typename T,
          absl::enable_if_t<std::is_same<T, void*>::value>* = nullptr>
T LuaGet(lua_State* L, int i) {
  return lua_touserdata(L, i);
}

template <
    typename T,
    absl::enable_if_t<
        !std::is_arithmetic<T>::value &&
        !std::is_constructible<T, const char*>::value &&
        !std::is_same<absl::decay_t<T>, ::y::LuaTable>::value &&
        !std::is_same<absl::decay_t<T>, ::y::LuaFunction>::value &&
        !std::is_pointer<T>::value &&
        (!std::is_reference<T>::value ||
         std::is_lvalue_reference<T>::value &&
             std::is_const<absl::remove_reference_t<T>>::value)>* = nullptr>
T LuaGet(lua_State* L, int i) {
  using Base = absl::decay_t<T>;
  luaL_argcheck(L, lua_getmetatable(L, i) != 0, i, "type has no metadata");
  ::y_internal::LuaPushMetatable<Base>(L);
  if (!lua_equal(L, -1, -2)) {
    lua_pop(L, 1);
    ::y_internal::LuaPushMetatable<Base*>(L);
    if (!lua_equal(L, -1, -2)) {
      lua_pop(L, 1);
      ::y_internal::LuaPushMetatable<const Base*>(L);
      luaL_argcheck(L, lua_equal(L, -1, -2) == 1, i, "invalid type detected");
    }
    lua_pop(L, 2);
    return std::forward<T>(**reinterpret_cast<Base**>(lua_touserdata(L, i)));
  }
  lua_pop(L, 2);
  return std::forward<T>(*reinterpret_cast<Base*>(lua_touserdata(L, i)));
}

template <typename T,
          absl::enable_if_t<
              !std::is_arithmetic<T>::value &&
              !std::is_constructible<T, const char*>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaTable>::value &&
              !std::is_same<absl::decay_t<T>, ::y::LuaFunction>::value &&
              !std::is_pointer<T>::value && std::is_reference<T>::value &&
              !(std::is_lvalue_reference<T>::value &&
                std::is_const<absl::remove_reference_t<T>>::value)>* = nullptr>
T LuaGet(lua_State* L, int i) {
  using Base = absl::decay_t<T>;
  luaL_argcheck(L, lua_getmetatable(L, i) != 0, i, "type has no metadata");
  ::y_internal::LuaPushMetatable<Base>(L);
  if (!lua_equal(L, -1, -2)) {
    lua_pop(L, 1);
    ::y_internal::LuaPushMetatable<Base*>(L);
    if (!lua_equal(L, -1, -2)) {
      lua_pop(L, 2);
      luaL_argerror(L, i, "invalid type, possibly const mismatch");
    }
    lua_pop(L, 2);
    return std::forward<T>(**reinterpret_cast<Base**>(lua_touserdata(L, i)));
  }
  lua_pop(L, 2);
  return std::forward<T>(*reinterpret_cast<Base*>(lua_touserdata(L, i)));
}

template <typename Arg>
void LuaPush(lua_State* L, Arg&& arg) {
  ::y_internal::LuaPush(L, std::forward<Arg>(arg));
}

template <typename ArgZero, typename... Args>
void LuaPush(lua_State* L, ArgZero&& arg, Args&&... args) {
  ::y_internal::LuaPush(L, std::forward<ArgZero>(arg));
  ::y::LuaPush(L, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
T* LuaEmplace(lua_State* L, Args&&... args) {
  void* t = lua_newuserdata(L, sizeof(T));
  ::new (t) T(std::forward<Args>(args)...);
  ::y_internal::LuaPushMetatable<T>(L);
  lua_setmetatable(L, -2);
  return static_cast<T*>(t);
}

}  // namespace y
#endif  // GAMMA_SCRIPTING_LUA_STACK_HPP_
