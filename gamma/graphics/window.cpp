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

#include "gamma/graphics/window.hpp"

#include "gamma/common/log.hpp"

namespace y {
namespace {

GLFWwindow* MakeWindow(const WindowSettings& settings) {
  YERR_IF(!(settings.width() > 0 && settings.height() > 0))
      << "Only windowed mode currently supported, size required.";

  auto* window = glfwCreateWindow(settings.width(), settings.height(),
                                  settings.title().c_str(), nullptr, nullptr);
  YERR_IF(window == nullptr) << "Forgot to initialize graphics?";
  return window;
}

}  // namespace

Window::Window(const WindowSettings& settings)
    : glfw_window_(MakeWindow(settings)), vulkan_context_(glfw_window_.get()) {}

void Window::display() {}

}  // namespace y
