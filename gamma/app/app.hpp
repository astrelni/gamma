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

#ifndef GAMMA_APP_APP_HPP_
#define GAMMA_APP_APP_HPP_

#include "gamma/app/app_settings.pb.h"
#include "gamma/app/window.hpp"
#include "gamma/common/function_queue.hpp"
#include "gamma/common/glfw.hpp"

namespace y {

class App {
 public:
  enum class Event {};

  App() = default;

  void init(const AppSettings& settings);

  void run();

  void signalClose() { signal_close_ = true; }

  // void setEventCallback(Event event, Function f);

  void callAfter(absl::Duration delay, Function<void()> f);
  void callEvery(absl::Duration interval, Function<void()> f);

 private:
  GlfwState glfw_state_;
  std::unique_ptr<Window> window_;
  bool signal_close_ = false;
  FunctionQueue function_queue_;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline void App::callAfter(absl::Duration delay, Function<void()> f) {
  function_queue_.callAfter(delay, std::move(f));
}

inline void App::callEvery(absl::Duration interval, Function<void()> f) {
  function_queue_.callEvery(interval, std::move(f));
}

}  // namespace y
#endif  // GAMMA_APP_APP_HPP_
