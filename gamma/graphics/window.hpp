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

#ifndef GAMMA_GRAPHICS_WINDOW_HPP_
#define GAMMA_GRAPHICS_WINDOW_HPP_

#include <memory>

#include "gamma/graphics/vk/context.hpp"
#include "gamma/graphics/vk/glfw.hpp"
#include "gamma/graphics/window_settings.pb.h"

namespace y {

class Window {
 public:
  static PollEvents();

  explicit Window(const WindowSettings& settings);

  bool shouldClose() const;

  void display();

 private:
  std::unique_ptr<GLFWwindow, GLFWWindowReleaser> glfw_window_;
  VulkanContext vulkan_context_;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline void Window::PollEvents() { glfwPollEvents(); }

inline bool Window::shouldClose() const {
  return glfwWindowShouldClose(glfw_window_.get());
}

}  // namespace y
#endif  // GAMMA_GRAPHICS_WINDOW_HPP_
