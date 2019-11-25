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

#ifndef GAMMA_GRAPHICS_VK_SEMAPHORE_HPP_
#define GAMMA_GRAPHICS_VK_SEMAPHORE_HPP_

#include "gamma/common/log.hpp"
#include "gamma/graphics/vk/device.hpp"

namespace y {

// RAII wrapper over VkSemaphore.
class VulkanSemaphore {
 public:
  explicit VulkanSemaphore(const VulkanDevice& device);
  ~VulkanSemaphore();

  VkSemaphore handle() const;

 private:
  VkDevice logical_device_;
  VkSemaphore semaphore_;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline VulkanSemaphore::VulkanSemaphore(const VulkanDevice& device)
    : logical_device_(device.logicalHandle()) {
  VkSemaphoreCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  YERR_IF(vkCreateSemaphore(logical_device_, &info, nullptr, &semaphore_) !=
          VK_SUCCESS);
}

inline VulkanSemaphore::~VulkanSemaphore() {
  vkDestroySemaphore(logical_device_, semaphore_, nullptr);
}

}  // namespace y
#endif  // GAMMA_GRAPHICS_VK_SEMAPHORE_HPP_
