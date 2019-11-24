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

#include "gamma/graphics/vk/swapchain.hpp"

#include <cstdint>

namespace y {
namespace {

std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(
    VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
  uint32_t n_formats;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &n_formats,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> formats(n_formats);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &n_formats,
                                       formats.data());
  return formats;
}

bool SurfaceHasNoPreferredFormat(
    const std::vector<VkSurfaceFormatKHR> formats) {
  return formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED;
}

VkSurfaceFormatKHR SelectSurfaceFormat(VkPhysicalDevice physical_device,
                                       VkSurfaceKHR surface) {
  std::vector<VkSurfaceFormatKHR> formats =
      GetSurfaceFormats(physical_device, surface);

  if (SurfaceHasNoPreferredFormat(formats)) {
    return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }

  for (const VkSurfaceFormatKHR& format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }
  return formats[0];
}

uint32_t Saturate(uint32_t num, uint32_t min, uint32_t max) {
  return num < min ? min : num > max ? max : num;
}

VkExtent2D Saturate(VkExtent2D extent, VkExtent2D min, VkExtent2D max) {
  return {Saturate(extent.width, min.width, max.width),
          Saturate(extent.height, min.height, max.height)};
}

VkExtent2D SelectExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                        GLFWwindow* window) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  int w, h;
  glfwGetWindowPos(window, &w, &h);
  return Saturate({static_cast<uint32_t>(w), static_cast<uint32_t>(h)},
                  capabilities.minImageExtent, capabilities.maxImageExtent);
}

uint32_t SelectMinImageCount(const VkSurfaceCapabilitiesKHR& capabilities) {
  uint32_t image_count = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount != 0 &&
      image_count > capabilities.maxImageCount) {
    return capabilities.maxImageCount;
  }
  return image_count;
}

}  // namespace

VulkanSwapchain::VulkanSwapchain(const VulkanDevice& device,
                                 const VulkanSurface& surface,
                                 GLFWwindow* window)
    : physical_device_(device.physicalHandle()),
      logical_device_(device.logicalHandle()) {
  VkSurfaceCapabilitiesKHR surface_capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_, surface.handle(),
                                            &surface_capabilities);
  VkSurfaceFormatKHR surface_format =
      SelectSurfaceFormat(physical_device_, surface.handle());

  format_ = surface_format.format;
  extent_ = SelectExtent(surface_capabilities, window);

  VkSwapchainCreateInfoKHR create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface.handle();
  create_info.minImageCount = SelectMinImageCount(surface_capabilities);
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent_;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t concurrent_indices[] = {device.graphicsQueue().family_index,
                                   device.presentQueue().family_index};
  if (concurrent_indices[0] != concurrent_indices[1]) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = concurrent_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  create_info.preTransform = surface_capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  YERR_IF(vkCreateSwapchainKHR(logical_device_, &create_info, nullptr,
                               &swapchain_) != VK_SUCCESS);

  uint32_t image_count;
  vkGetSwapchainImagesKHR(logical_device_, swapchain_, &image_count, nullptr);
  images_.resize(image_count);
  vkGetSwapchainImagesKHR(logical_device_, swapchain_, &image_count,
                          images_.data());

  image_views_.resize(images_.size());
  for (size_t i = 0; i < images_.size(); ++i) {
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = images_[i];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format_;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    YERR_IF(vkCreateImageView(logical_device_, &create_info, nullptr,
                              &image_views_[i]) != VK_SUCCESS);
  }
}

}  // namespace y
