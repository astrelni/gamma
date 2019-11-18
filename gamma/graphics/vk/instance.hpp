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

#ifndef GAMMA_GRAPHICS_VK_INSTANCE_HPP_
#define GAMMA_GRAPHICS_VK_INSTANCE_HPP_

#include "gamma/common/log.hpp"
#include "gamma/graphics/vk/glfw.hpp"

namespace y {

// Simple RAII wrapper over VkInstance.
class VulkanInstance {
 public:
  VulkanInstance();
  ~VulkanInstance();

  VkInstance handle() const;

 private:
  VkInstance instance_;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline VulkanInstance::VulkanInstance() : instance_(VK_NULL_HANDLE) {
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.ppEnabledExtensionNames =
      glfwGetRequiredInstanceExtensions(&create_info.enabledExtensionCount);
  create_info.enabledLayerCount = 0;
  YERR_IF(vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS);
}

inline VulkanInstance::~VulkanInstance() {
  vkDestroyInstance(instance_, nullptr);
}

inline VkInstance VulkanInstance::handle() const { return instance_; }

}  // namespace y
#endif  // GAMMA_GRAPHICS_VK_STATE_HPP_
