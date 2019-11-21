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

#ifndef GAMMA_ENGINE_ENGINE_HPP_
#define GAMMA_ENGINE_ENGINE_HPP_

#include "gamma/common/function_queue.hpp"
#include "gamma/engine/engine_settings.pb.h"
#include "gamma/graphics/vk/glfw.hpp"
#include "gamma/graphics/window.hpp"

namespace y {

class Engine {
 public:
  enum class Event {};

  explicit Engine(const EngineSettings& settings);

  void runMainLoop();

  void signalClose() { signal_close_ = true; }

  // void setEventCallback(Event event, Function f);

  void setTimeout(Function<void()> f, absl::Duration delay);

 private:
  Window window_;
  bool signal_close_;
  FunctionQueue function_queue_;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline void Engine::setTimeout(Function<void()> f, absl::Duration delay) {
  function_queue_.setTimeout(std::move(f), delay);
}

}  // namespace y
#endif  // GAMMA_ENGINE_ENGINE_HPP_
