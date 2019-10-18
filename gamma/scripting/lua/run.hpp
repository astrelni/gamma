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

#ifndef GAMMA_RUNTIME_LUA_RUN_HPP_
#define GAMMA_RUNTIME_LUA_RUN_HPP_

#include <string>

#include "gamma/common/log.hpp"
#include "gamma/runtime/lua/call.hpp"
#include "lua.hpp"

namespace y {

void LuaRunFile(lua_State* L, const std::string& file);
void LuaRunFile(lua_State* L, const std::string& file, const LuaTable& env);

void LuaRunSource(lua_State* L, const std::string& name, absl::string_view src);
void LuaRunSource(lua_State* L, const std::string& name, absl::string_view src,
                  const LuaTable& env);

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline void LuaRunFile(lua_State* L, const std::string& file) {
  YERR_IF(luaL_loadfile(L, file.c_str()))
      << "failed to open file '" << file << "'";
  LuaPCall(L, 0);
}

inline void LuaRunFile(lua_State* L, const std::string& file,
                       const LuaTable& env) {
  YERR_IF(luaL_loadfile(L, file.c_str()))
      << "failed to open file '" << file << "'";
  YERR_IF(L != env.rawState())
      << "environment table lua_State does not match requested state for file";
  env.push();
  lua_setfenv(L, -2);
  LuaPCall(L, 0);
}

inline void LuaRunSource(lua_State* L, const std::string& name,
                         absl::string_view src) {
  YERR_IF(luaL_loadbuffer(L, src.data(), src.length(), name.c_str()))
      << "failed to load source buffer string";
  LuaPCall(L, 0);
}

inline void LuaRunSource(lua_State* L, const std::string& name,
                         absl::string_view src, const LuaTable& env) {
  YERR_IF(luaL_loadbuffer(L, src.data(), src.length(), name.c_str()))
      << "failed to load source string";
  YERR_IF(L != env.rawState()) << "environment table lua state does not match "
                                  "requested state for source string";
  env.push();
  lua_setfenv(L, -2);
  LuaPCall(L, 0);
}

}  // namespace y
#endif  // GAMMA_RUNTIME_LUA_RUN_HPP_
