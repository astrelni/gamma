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

#ifndef GAMMA_GRAPHICS_VK_DEVICE_HPP_
#define GAMMA_GRAPHICS_VK_DEVICE_HPP_

#include "gamma/graphics/vk/instance.hpp"
#include "gamma/graphics/vk/queue.hpp"
#include "gamma/graphics/vk/surface.hpp"

namespace y {

class VulkanDevice {
 public:
  VulkanDevice(const VulkanInstance& instance, const VulkanSurface& surface);
  ~VulkanDevice();

  VkPhysicalDevice physicalHandle() const;
  VkDevice logicalHandle() const;

  const VulkanQueue& graphicsQueue() const;
  const VulkanQueue& computeQueue() const;
  const VulkanQueue& transferQueue() const;
  const VulkanQueue& presentQueue() const;

 private:
  VkPhysicalDevice physical_device_;
  VkDevice logical_device_;
  VulkanQueue graphics_queue_;
  VulkanQueue compute_queue_;
  VulkanQueue transfer_queue_;
  VulkanQueue present_queue_;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline VulkanDevice::~VulkanDevice() {
  vkDestroyDevice(logical_device_, nullptr);
}

inline VkPhysicalDevice VulkanDevice::physicalHandle() const {
  return physical_device_;
}

inline VkDevice VulkanDevice::logicalHandle() const { return logical_device_; }

inline const VulkanQueue& VulkanDevice::graphicsQueue() const {
  return graphics_queue_;
}

inline const VulkanQueue& VulkanDevice::computeQueue() const {
  return compute_queue_;
}

inline const VulkanQueue& VulkanDevice::transferQueue() const {
  return transfer_queue_;
}

inline const VulkanQueue& VulkanDevice::presentQueue() const {
  return present_queue_;
}

}  // namespace y
#endif  // GAMMA_GRAPHICS_VK_DEVICE_HPP_
