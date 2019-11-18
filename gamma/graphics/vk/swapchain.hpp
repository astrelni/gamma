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

#ifndef GAMMA_GRAPHICS_VK_SWAPCHAIN_HPP_
#define GAMMA_GRAPHICS_VK_SWAPCHAIN_HPP_

#include <vector>

#include "gamma/graphics/vk/device.hpp"
#include "gamma/graphics/vk/glfw.hpp"
#include "gamma/graphics/vk/surface.hpp"

namespace y {

class VulkanSwapchain {
 public:
  VulkanSwapchain(const VulkanDevice& device, const VulkanSurface& surface,
                  GLFWwindow* window);
  ~VulkanSwapchain();

 private:
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice logical_device_ = VK_NULL_HANDLE;
  VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  VkFormat format_;
  VkExtent2D extent_;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

inline VulkanSwapchain::~VulkanSwapchain() {
  for (auto view : image_views_) {
    vkDestroyImageView(logical_device_, view, nullptr);
  }
  vkDestroySwapchainKHR(logical_device_, swapchain_, nullptr);
}

}  // namespace y
#endif  // GAMMA_GRAPHICS_VK_SWAPCHAIN_HPP_
