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

#include "gamma/graphics/vk/glfw.hpp"

#include "gamma/common/log.hpp"

namespace y {
namespace {

absl::string_view HumanReadableErrorCode(int code) {
  switch (code) {
    case GLFW_NOT_INITIALIZED:
      return "GLFW_NOT_INITIALIZED";
    case GLFW_NO_CURRENT_CONTEXT:
      return "GLFW_NO_CURRENT_CONTEXT";
    case GLFW_INVALID_ENUM:
      return "GLFW_INVALID_ENUM";
    case GLFW_INVALID_VALUE:
      return "GLFW_INVALID_VALUE";
    case GLFW_OUT_OF_MEMORY:
      return "GLFW_OUT_OF_MEMORY";
    case GLFW_API_UNAVAILABLE:
      return "GLFW_API_UNAVAILABLE";
    case GLFW_VERSION_UNAVAILABLE:
      return "GLFW_VERSION_UNAVAILABLE";
    case GLFW_PLATFORM_ERROR:
      return "GLFW_PLATFORM_ERROR";
    case GLFW_FORMAT_UNAVAILABLE:
      return "GLFW_FORMAT_UNAVAILABLE";
    case GLFW_NO_WINDOW_CONTEXT:
      return "GLFW_NO_WINDOW_CONTEXT";
    default:
      break;
  }
  return "";
}

}  // namespace

void GLFWInit() {
  YERR_IF(glfwInit() != GLFW_TRUE);

  // Bubble glfw errors to logging:
  GLFWerrorfun callback = [](int code, const char* /* unused */) {
    YERR << "GLFW reported error code " << HumanReadableErrorCode(code);
  };
  (void)glfwSetErrorCallback(callback);

  // Use Vulkan instead of OpenGL:
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

}  // namespace y
