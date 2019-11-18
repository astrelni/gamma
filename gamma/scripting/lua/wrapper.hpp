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

#ifndef GAMMA_SCRIPTING_LUA_WRAPPER_HPP_
#define GAMMA_SCRIPTING_LUA_WRAPPER_HPP_

#include "gamma/scripting/lua/stack.hpp"
#include "lua.hpp"

// Compile-time generate a lua_CFunction from a C++ function. The C++ function
// can be free or a member, static or non-static, const or non-const qualfied,
// and can be void or some type returning. The main limitation of this macro is
// that it cannot be part of an overload set. In that case, you can manually
// specify the function signature by directly spelling LuaWrapper::Function
// instead of using this macro.
//
// It is required that any types related to the function (return type, parameter
// type or class type for member functions) be declared with
// GAMMA_CREATE_LUA_METATABLE_KEYS(<Type>) (from engine/lua/stack.hpp) before
// the function is exposed to lua.
//
// The primary use for this macro is inside calls to LuaExpose* found in
// engine/lua/exposure.hpp
#define GAMMA_LUA_CFUNCTION(function) \
  (::y::LuaWrapper<decltype(&function)>::Function<&function>())

// To generate a wrapper for a constructor, use e.g.:
//
//   y::LuaWrapper<MyType(int, float)>::Constructor()
//

// -----------------------------------------------------------------------------
//                  Implementation Details (Here be dragons)

namespace y_internal {

int LuaWrongNumberOfArguments(lua_State* L, int expected);

}  // namespace y_internal

namespace y {

#define GAMMA_LUA_FREE_ARG1(t1) ::y::LuaGet<t1>(L, 1)
#define GAMMA_LUA_FREE_ARG2(t1, t2) ::y::LuaGet<t1>(L, 1), ::y::LuaGet<t2>(L, 2)
#define GAMMA_LUA_FREE_ARG3(t1, t2, t3) \
  GAMMA_LUA_FREE_ARG2(t1, t2), ::y::LuaGet<t3>(L, 3)
#define GAMMA_LUA_FREE_ARG4(t1, t2, t3, t4) \
  GAMMA_LUA_FREE_ARG3(t1, t2, t3), ::y::LuaGet<t4>(L, 4)
#define GAMMA_LUA_FREE_ARG5(t1, t2, t3, t4, t5) \
  GAMMA_LUA_FREE_ARG4(t1, t2, t3, t4), ::y::LuaGet<t5>(L, 5)
#define GAMMA_LUA_FREE_ARG6(t1, t2, t3, t4, t5, t6) \
  GAMMA_LUA_FREE_ARG5(t1, t2, t3, t4, t5), ::y::LuaGet<t6>(L, 6)
#define GAMMA_LUA_FREE_ARG7(t1, t2, t3, t4, t5, t6, t7) \
  GAMMA_LUA_FREE_ARG6(t1, t2, t3, t4, t5, t6), ::y::LuaGet<t7>(L, 7)
#define GAMMA_LUA_FREE_ARG8(t1, t2, t3, t4, t5, t6, t7, t8) \
  GAMMA_LUA_FREE_ARG7(t1, t2, t3, t4, t5, t6, t7), ::y::LuaGet<t8>(L, 8)

#define GAMMA_LUA_MEMBER_ARG1(t1) ::y::LuaGet<t1>(L, 2)
#define GAMMA_LUA_MEMBER_ARG2(t1, t2) \
  GAMMA_LUA_MEMBER_ARG1(t1), ::y::LuaGet<t2>(L, 3)
#define GAMMA_LUA_MEMBER_ARG3(t1, t2, t3) \
  GAMMA_LUA_MEMBER_ARG2(t1, t2), ::y::LuaGet<t3>(L, 4)
#define GAMMA_LUA_MEMBER_ARG4(t1, t2, t3, t4) \
  GAMMA_LUA_MEMBER_ARG3(t1, t2, t3), ::y::LuaGet<t4>(L, 5)
#define GAMMA_LUA_MEMBER_ARG5(t1, t2, t3, t4, t5) \
  GAMMA_LUA_MEMBER_ARG4(t1, t2, t3, t4), ::y::LuaGet<t5>(L, 6)
#define GAMMA_LUA_MEMBER_ARG6(t1, t2, t3, t4, t5, t6) \
  GAMMA_LUA_MEMBER_ARG5(t1, t2, t3, t4, t5), ::y::LuaGet<t6>(L, 7)
#define GAMMA_LUA_MEMBER_ARG7(t1, t2, t3, t4, t5, t6, t7) \
  GAMMA_LUA_MEMBER_ARG6(t1, t2, t3, t4, t5, t6), ::y::LuaGet<t7>(L, 8)
#define GAMMA_LUA_MEMBER_ARG8(t1, t2, t3, t4, t5, t6, t7, t8) \
  GAMMA_LUA_MEMBER_ARG7(t1, t2, t3, t4, t5, t6, t7), ::y::LuaGet<t8>(L, 9)

#define GAMMA_LUA_CHOOSE(_1, _2, _3, _4, _5, _6, _7, _8, x, ...) x

#define GAMMA_LUA_FREE_ARGS(...)                                          \
  GAMMA_LUA_CHOOSE(__VA_ARGS__, GAMMA_LUA_FREE_ARG8, GAMMA_LUA_FREE_ARG7, \
                   GAMMA_LUA_FREE_ARG6, GAMMA_LUA_FREE_ARG5,              \
                   GAMMA_LUA_FREE_ARG4, GAMMA_LUA_FREE_ARG3,              \
                   GAMMA_LUA_FREE_ARG2, GAMMA_LUA_FREE_ARG1)              \
  (__VA_ARGS__)
#define GAMMA_LUA_MEMBER_ARGS(...)                                            \
  GAMMA_LUA_CHOOSE(__VA_ARGS__, GAMMA_LUA_MEMBER_ARG8, GAMMA_LUA_MEMBER_ARG7, \
                   GAMMA_LUA_MEMBER_ARG6, GAMMA_LUA_MEMBER_ARG5,              \
                   GAMMA_LUA_MEMBER_ARG4, GAMMA_LUA_MEMBER_ARG3,              \
                   GAMMA_LUA_MEMBER_ARG2, GAMMA_LUA_MEMBER_ARG1)              \
  (__VA_ARGS__)

#define GAMMA_LUA_TYPENAME1(t1) typename t1
#define GAMMA_LUA_TYPENAME2(t1, t2) typename t1, typename t2
#define GAMMA_LUA_TYPENAME3(t1, t2, t3) GAMMA_LUA_TYPENAME2(t1, t2), typename t3
#define GAMMA_LUA_TYPENAME4(t1, t2, t3, t4) \
  GAMMA_LUA_TYPENAME3(t1, t2, t3), typename t4
#define GAMMA_LUA_TYPENAME5(t1, t2, t3, t4, t5) \
  GAMMA_LUA_TYPENAME4(t1, t2, t3, t4), typename t5
#define GAMMA_LUA_TYPENAME6(t1, t2, t3, t4, t5, t6) \
  GAMMA_LUA_TYPENAME5(t1, t2, t3, t4, t5), typename t6
#define GAMMA_LUA_TYPENAME7(t1, t2, t3, t4, t5, t6, t7) \
  GAMMA_LUA_TYPENAME6(t1, t2, t3, t4, t5, t6), typename t7
#define GAMMA_LUA_TYPENAME8(t1, t2, t3, t4, t5, t6, t7, t8) \
  GAMMA_LUA_TYPENAME7(t1, t2, t3, t4, t5, t6, t7), typename t8

#define GAMMA_LUA_TYPENAMES(...)                                          \
  GAMMA_LUA_CHOOSE(__VA_ARGS__, GAMMA_LUA_TYPENAME8, GAMMA_LUA_TYPENAME7, \
                   GAMMA_LUA_TYPENAME6, GAMMA_LUA_TYPENAME5,              \
                   GAMMA_LUA_TYPENAME4, GAMMA_LUA_TYPENAME3,              \
                   GAMMA_LUA_TYPENAME2, GAMMA_LUA_TYPENAME1)              \
  (__VA_ARGS__)

#define GAMMA_LUA_COUNT_FREE_ARGS(...) \
  GAMMA_LUA_CHOOSE(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1)
#define GAMMA_LUA_COUNT_MEMBER_ARGS(...) \
  GAMMA_LUA_CHOOSE(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2)

template <typename>
struct LuaWrapper;

template <>
struct LuaWrapper<void()> {
  using FuncType = void (*)();

  template <FuncType Func>
  constexpr static lua_CFunction Function() {
    return [](lua_State* L) -> int {
      if (lua_gettop(L) != 0) {
        return ::y_internal::LuaWrongNumberOfArguments(L, 0);
      }
      Func();
      return 0;
    };
  }
};

#define GAMMA_LUA_VOID_FREE_FUNCTION(...)                              \
  template <GAMMA_LUA_TYPENAMES(__VA_ARGS__)>                          \
  struct LuaWrapper<void(__VA_ARGS__)> {                               \
    using FuncType = void (*)(__VA_ARGS__);                            \
                                                                       \
    template <FuncType Func>                                           \
    constexpr static lua_CFunction Function() {                        \
      return [](lua_State* L) -> int {                                 \
        if (lua_gettop(L) != GAMMA_LUA_COUNT_FREE_ARGS(__VA_ARGS__)) { \
          return ::y_internal::LuaWrongNumberOfArguments(              \
              L, GAMMA_LUA_COUNT_FREE_ARGS(__VA_ARGS__));              \
        }                                                              \
        Func(GAMMA_LUA_FREE_ARGS(__VA_ARGS__));                        \
        lua_settop(L, 0);                                              \
        return 0;                                                      \
      };                                                               \
    }                                                                  \
  }

GAMMA_LUA_VOID_FREE_FUNCTION(P1);
GAMMA_LUA_VOID_FREE_FUNCTION(P1, P2);
GAMMA_LUA_VOID_FREE_FUNCTION(P1, P2, P3);
GAMMA_LUA_VOID_FREE_FUNCTION(P1, P2, P3, P4);
GAMMA_LUA_VOID_FREE_FUNCTION(P1, P2, P3, P4, P5);
GAMMA_LUA_VOID_FREE_FUNCTION(P1, P2, P3, P4, P5, P6);
GAMMA_LUA_VOID_FREE_FUNCTION(P1, P2, P3, P4, P5, P6, P7);
GAMMA_LUA_VOID_FREE_FUNCTION(P1, P2, P3, P4, P5, P6, P7, P8);
#undef GAMMA_LUA_VOID_FREE_FUNCTION

template <typename R>
struct LuaWrapper<R()> {
  using FuncType = R (*)();

  template <FuncType Func>
  constexpr static lua_CFunction Function() {
    return [](lua_State* L) -> int {
      if (lua_gettop(L) != 0) {
        return ::y_internal::LuaWrongNumberOfArguments(L, 0);
      }
      ::y::LuaPush(L, std::forward<R>(Func()));
      return 1;
    };
  }

  constexpr static lua_CFunction Constructor() {
    return [](lua_State* L) -> int {
      ::y::LuaEmplace<R>(L);
      return 1;
    };
  }
};

#define GAMMA_LUA_R_FREE_FUNCTION(...)                                         \
  template <typename R, GAMMA_LUA_TYPENAMES(__VA_ARGS__)>                      \
  struct LuaWrapper<R(__VA_ARGS__)> {                                          \
    using FuncType = R (*)(__VA_ARGS__);                                       \
                                                                               \
    template <FuncType Func>                                                   \
    constexpr static lua_CFunction Function() {                                \
      return [](lua_State* L) -> int {                                         \
        if (lua_gettop(L) != GAMMA_LUA_COUNT_FREE_ARGS(__VA_ARGS__)) {         \
          return ::y_internal::LuaWrongNumberOfArguments(                      \
              L, GAMMA_LUA_COUNT_FREE_ARGS(__VA_ARGS__));                      \
        }                                                                      \
        ::y::LuaPush(L,                                                        \
                     std::forward<R>(Func(GAMMA_LUA_FREE_ARGS(__VA_ARGS__)))); \
        lua_replace(L, 1);                                                     \
        lua_settop(L, 1);                                                      \
        return 1;                                                              \
      };                                                                       \
    }                                                                          \
                                                                               \
    constexpr static lua_CFunction Constructor() {                             \
      return [](lua_State* L) -> int {                                         \
        ::y::LuaEmplace<R>(L, GAMMA_LUA_FREE_ARGS(__VA_ARGS__));               \
        lua_replace(L, 1);                                                     \
        lua_settop(L, 1);                                                      \
        return 1;                                                              \
      };                                                                       \
    }                                                                          \
  }

GAMMA_LUA_R_FREE_FUNCTION(P1);
GAMMA_LUA_R_FREE_FUNCTION(P1, P2);
GAMMA_LUA_R_FREE_FUNCTION(P1, P2, P3);
GAMMA_LUA_R_FREE_FUNCTION(P1, P2, P3, P4);
GAMMA_LUA_R_FREE_FUNCTION(P1, P2, P3, P4, P5);
GAMMA_LUA_R_FREE_FUNCTION(P1, P2, P3, P4, P5, P6);
GAMMA_LUA_R_FREE_FUNCTION(P1, P2, P3, P4, P5, P6, P7);
GAMMA_LUA_R_FREE_FUNCTION(P1, P2, P3, P4, P5, P6, P7, P8);
#undef GAMMA_LUA_R_FREE_FUNCTION

template <typename T>
struct LuaWrapper<void (T::*)()> {
  using FuncType = void (T::*)();

  template <FuncType Func>
  constexpr static lua_CFunction Function() {
    return [](lua_State* L) -> int {
      if (lua_gettop(L) != 1) {
        return ::y_internal::LuaWrongNumberOfArguments(L, 1);
      }
      (::y::LuaGet<T*>(L, 1)->*Func)();
      lua_settop(L, 0);
      return 0;
    };
  }
};

#define GAMMA_LUA_VOID_MEMBER_FUNCTION(...)                                 \
  template <typename T, GAMMA_LUA_TYPENAMES(__VA_ARGS__)>                   \
  struct LuaWrapper<void (T::*)(__VA_ARGS__)> {                             \
    using FuncType = void (T::*)(__VA_ARGS__);                              \
                                                                            \
    template <FuncType Func>                                                \
    constexpr static lua_CFunction Function() {                             \
      return [](lua_State* L) -> int {                                      \
        if (lua_gettop(L) != GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__)) {    \
          return ::y_internal::LuaWrongNumberOfArguments(                   \
              L, GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__));                 \
        }                                                                   \
        (::y::LuaGet<T*>(L, 1)->*Func)(GAMMA_LUA_MEMBER_ARGS(__VA_ARGS__)); \
        lua_settop(L, 0);                                                   \
        return 0;                                                           \
      };                                                                    \
    }                                                                       \
  }

GAMMA_LUA_VOID_MEMBER_FUNCTION(P1);
GAMMA_LUA_VOID_MEMBER_FUNCTION(P1, P2);
GAMMA_LUA_VOID_MEMBER_FUNCTION(P1, P2, P3);
GAMMA_LUA_VOID_MEMBER_FUNCTION(P1, P2, P3, P4);
GAMMA_LUA_VOID_MEMBER_FUNCTION(P1, P2, P3, P4, P5);
GAMMA_LUA_VOID_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6);
GAMMA_LUA_VOID_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7);
GAMMA_LUA_VOID_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7, P8);
#undef GAMMA_LUA_VOID_MEMBER_FUNCTION

template <typename T>
struct LuaWrapper<void (T::*)() const> {
  using FuncType = void (T::*)() const;

  template <FuncType Func>
  constexpr static lua_CFunction Function() {
    return [](lua_State* L) -> int {
      if (lua_gettop(L) != 1) {
        return ::y_internal::LuaWrongNumberOfArguments(L, 1);
      }
      (::y::LuaGet<const T*>(L, 1)->*Func)();
      lua_settop(L, 0);
      return 0;
    };
  }
};

#define GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(...)                        \
  template <typename T, GAMMA_LUA_TYPENAMES(__VA_ARGS__)>                \
  struct LuaWrapper<void (T::*)(__VA_ARGS__) const> {                    \
    using FuncType = void (T::*)(__VA_ARGS__) const;                     \
                                                                         \
    template <FuncType Func>                                             \
    constexpr static lua_CFunction Function() {                          \
      return [](lua_State* L) -> int {                                   \
        if (lua_gettop(L) != GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__)) { \
          return ::y_internal::LuaWrongNumberOfArguments(                \
              L, GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__));              \
        }                                                                \
        (::y::LuaGet<const T*>(L, 1)->*Func)(                            \
            GAMMA_LUA_MEMBER_ARGS(__VA_ARGS__));                         \
        lua_settop(L, 0);                                                \
        return 0;                                                        \
      };                                                                 \
    }                                                                    \
  }

GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1);
GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1, P2);
GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1, P2, P3);
GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1, P2, P3, P4);
GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5);
GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6);
GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7);
GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7, P8);
#undef GAMMA_LUA_VOID_CONST_MEMBER_FUNCTION

template <typename R, typename T>
struct LuaWrapper<R (T::*)()> {
  using FuncType = R (T::*)();

  template <FuncType Func>
  constexpr static lua_CFunction Function() {
    return [](lua_State* L) -> int {
      if (lua_gettop(L) != 1) {
        return ::y_internal::LuaWrongNumberOfArguments(L, 1);
      }
      ::y::LuaPush(L, std::forward<R>((::y::LuaGet<T*>(L, 1)->*Func)()));
      lua_replace(L, 1);
      lua_settop(L, 1);
      return 1;
    };
  }
};

#define GAMMA_LUA_R_MEMBER_FUNCTION(...)                                 \
  template <typename R, typename T, GAMMA_LUA_TYPENAMES(__VA_ARGS__)>    \
  struct LuaWrapper<R (T::*)(__VA_ARGS__)> {                             \
    using FuncType = R (T::*)(__VA_ARGS__);                              \
                                                                         \
    template <FuncType Func>                                             \
    constexpr static lua_CFunction Function() {                          \
      return [](lua_State* L) -> int {                                   \
        if (lua_gettop(L) != GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__)) { \
          return ::y_internal::LuaWrongNumberOfArguments(                \
              L, GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__));              \
        }                                                                \
        ::y::LuaPush(L, std::forward<R>((::y::LuaGet<T*>(L, 1)->*Func)(  \
                            GAMMA_LUA_MEMBER_ARGS(__VA_ARGS__))));       \
        lua_replace(L, 1);                                               \
        lua_settop(L, 1);                                                \
        return 1;                                                        \
      };                                                                 \
    }                                                                    \
  }

GAMMA_LUA_R_MEMBER_FUNCTION(P1);
GAMMA_LUA_R_MEMBER_FUNCTION(P1, P2);
GAMMA_LUA_R_MEMBER_FUNCTION(P1, P2, P3);
GAMMA_LUA_R_MEMBER_FUNCTION(P1, P2, P3, P4);
GAMMA_LUA_R_MEMBER_FUNCTION(P1, P2, P3, P4, P5);
GAMMA_LUA_R_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6);
GAMMA_LUA_R_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7);
GAMMA_LUA_R_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7, P8);
#undef GAMMA_LUA_R_MEMBER_FUNCTION

template <typename R, typename T>
struct LuaWrapper<R (T::*)() const> {
  using FuncType = R (T::*)() const;

  template <FuncType Func>
  constexpr static lua_CFunction Function() {
    return [](lua_State* L) -> int {
      if (lua_gettop(L) != 1) {
        return ::y_internal::LuaWrongNumberOfArguments(L, 1);
      }
      ::y::LuaPush(L, std::forward<R>((::y::LuaGet<const T*>(L, 1)->*Func)()));
      lua_replace(L, 1);
      lua_settop(L, 1);
      return 1;
    };
  }
};

#define GAMMA_LUA_R_CONST_MEMBER_FUNCTION(...)                                \
  template <typename R, typename T, GAMMA_LUA_TYPENAMES(__VA_ARGS__)>         \
  struct LuaWrapper<R (T::*)(__VA_ARGS__) const> {                            \
    using FuncType = R (T::*)(__VA_ARGS__) const;                             \
                                                                              \
    template <FuncType Func>                                                  \
    constexpr static lua_CFunction Function() {                               \
      return [](lua_State* L) -> int {                                        \
        if (lua_gettop(L) != GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__)) {      \
          return ::y_internal::LuaWrongNumberOfArguments(                     \
              L, GAMMA_LUA_COUNT_MEMBER_ARGS(__VA_ARGS__));                   \
        }                                                                     \
        ::y::LuaPush(L, std::forward<R>((::y::LuaGet<const T*>(L, 1)->*Func)( \
                            GAMMA_LUA_MEMBER_ARGS(__VA_ARGS__))));            \
        lua_replace(L, 1);                                                    \
        lua_settop(L, 1);                                                     \
        return 1;                                                             \
      };                                                                      \
    }                                                                         \
  }

GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1);
GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1, P2);
GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1, P2, P3);
GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1, P2, P3, P4);
GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5);
GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6);
GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7);
GAMMA_LUA_R_CONST_MEMBER_FUNCTION(P1, P2, P3, P4, P5, P6, P7, P8);
#undef GAMMA_LUA_R_CONST_MEMBER_FUNCTION

#undef GAMMA_LUA_COUNT_MEMBER_ARGS
#undef GAMMA_LUA_COUNT_FREE_ARGS
#undef GAMMA_LUA_TYPENAMES
#undef GAMMA_LUA_TYPENAME8
#undef GAMMA_LUA_TYPENAME7
#undef GAMMA_LUA_TYPENAME6
#undef GAMMA_LUA_TYPENAME5
#undef GAMMA_LUA_TYPENAME4
#undef GAMMA_LUA_TYPENAME3
#undef GAMMA_LUA_TYPENAME2
#undef GAMMA_LUA_TYPENAME1
#undef GAMMA_LUA_MEMBER_ARGS
#undef GAMMA_LUA_FREE_ARGS
#undef GAMMA_LUA_CHOOSE
#undef GAMMA_LUA_MEMBER_ARG8
#undef GAMMA_LUA_MEMBER_ARG7
#undef GAMMA_LUA_MEMBER_ARG6
#undef GAMMA_LUA_MEMBER_ARG5
#undef GAMMA_LUA_MEMBER_ARG4
#undef GAMMA_LUA_MEMBER_ARG3
#undef GAMMA_LUA_MEMBER_ARG2
#undef GAMMA_LUA_MEMBER_ARG1
#undef GAMMA_LUA_FREE_ARG8
#undef GAMMA_LUA_FREE_ARG7
#undef GAMMA_LUA_FREE_ARG6
#undef GAMMA_LUA_FREE_ARG5
#undef GAMMA_LUA_FREE_ARG4
#undef GAMMA_LUA_FREE_ARG3
#undef GAMMA_LUA_FREE_ARG2
#undef GAMMA_LUA_FREE_ARG1

}  // namespace y_internal
#endif  // GAMMA_SCRIPTING_LUA_WRAPPER_HPP_
