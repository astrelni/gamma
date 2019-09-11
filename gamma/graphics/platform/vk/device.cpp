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

#include "gamma/graphics/vk/device.hpp"

#include <cstdint>
#include <set>
#include <vector>

#include "absl/algorithm/container.h"
#include "absl/strings/string_view.h"

namespace y {
namespace {

bool DeviceSupportsQueueFlagBit(
    const std::vector<VkQueueFamilyProperties>& queue_families,
    VkQueueFlagBits bit) {
  for (const auto& queue_family_properties : queue_families) {
    if (queue_family_properties.queueFlags & bit) return true;
  }
  return false;
}

bool DeviceSupportsSurface(VkPhysicalDevice physical_device,
                           uint32_t num_queue_families, VkSurfaceKHR surface) {
  for (uint32_t i = 0; i < num_queue_families; ++i) {
    VkBool32 surface_supported;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface,
                                         &surface_supported);
    if (surface_supported) {
      uint32_t num_formats, num_modes;
      vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface,
                                           &num_formats, nullptr);
      vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                                &num_modes, nullptr);
      if (num_formats != 0 && num_modes != 0) return true;
    }
  }
  return false;
}

std::vector<const char*> GetRequiredExtensions() {
  return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

bool DeviceSupportsRequiredExtensions(VkPhysicalDevice physical_device) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                       &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(
      physical_device, nullptr, &extension_count, available_extensions.data());

  for (absl::string_view extension_name : GetRequiredExtensions()) {
    auto matches_extension_name =
        [extension_name](const VkExtensionProperties& extension) {
          return extension_name == extension.extensionName;
        };
    if (absl::c_find_if(available_extensions, matches_extension_name) ==
        available_extensions.end()) {
      return false;
    }
  }
  return true;
}

bool DeviceMeetsMinimumRequirements(VkPhysicalDevice physical_device,
                                    VkSurfaceKHR surface) {
  uint32_t queue_family_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                           nullptr);
  if (queue_family_count == 0) return false;

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                           queue_families.data());

  return DeviceSupportsQueueFlagBit(queue_families, VK_QUEUE_GRAPHICS_BIT) &&
         DeviceSupportsQueueFlagBit(queue_families, VK_QUEUE_COMPUTE_BIT) &&
         DeviceSupportsQueueFlagBit(queue_families, VK_QUEUE_TRANSFER_BIT) &&
         DeviceSupportsSurface(physical_device, queue_family_count, surface) &&
         DeviceSupportsRequiredExtensions(physical_device);
}

uint32_t DeviceScore(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
  if (physical_device == VK_NULL_HANDLE ||
      !DeviceMeetsMinimumRequirements(physical_device, surface)) {
    return 0;
  }

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physical_device, &properties);
  YERR_IF(properties.deviceType < 0 || properties.deviceType > 4);
  uint32_t device_type_scores[] = {
      1,  // VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
      4,  // VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
      5,  // VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
      3,  // VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
      2   // VK_PHYSICAL_DEVICE_TYPE_CPU = 4
  };
  return device_type_scores[properties.deviceType];
}

VkPhysicalDevice SelectPhysicalDevice(VkInstance instance,
                                      VkSurfaceKHR surface) {
  uint32_t device_count;
  vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
  YERR_IF(device_count == 0);
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

  VkPhysicalDevice best_device = VK_NULL_HANDLE;
  uint32_t best_score = 0;
  for (VkPhysicalDevice device : devices) {
    uint32_t score = DeviceScore(device, surface);
    if (score > best_score) {
      best_device = device;
      best_score = score;
    }
  }

  YERR_IF(best_device == VK_NULL_HANDLE)
      << "No graphics device meets minimum requirements";
  return best_device;
}

VkDevice CreateLogicalDeviceAndQueues(VkPhysicalDevice physical_device,
                                      VkSurfaceKHR surface,
                                      VulkanQueue* graphics_queue,
                                      VulkanQueue* compute_queue,
                                      VulkanQueue* transfer_queue,
                                      VulkanQueue* present_queue) {
  uint32_t queue_family_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                           nullptr);
  YERR_IF(queue_family_count == 0);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                           queue_families.data());

  bool found_graphics = false;
  bool found_compute = false;
  bool found_transfer = false;
  bool found_present = false;
  std::set<uint32_t> queue_families_to_create;
  for (uint32_t i = 0; i < queue_family_count; ++i) {
    if (!found_graphics &&
        queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      found_graphics = true;
      graphics_queue->family_index = i;
      queue_families_to_create.insert(i);
    }
    if (!found_compute && queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      found_compute = true;
      compute_queue->family_index = i;
      queue_families_to_create.insert(i);
    }
    if (!found_transfer &&
        queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
      found_transfer = true;
      transfer_queue->family_index = i;
      queue_families_to_create.insert(i);
    }
    if (!found_present) {
      VkBool32 present_support = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface,
                                           &present_support);
      if (present_support) {
        found_present = true;
        present_queue->family_index = i;
        queue_families_to_create.insert(i);
      }
    }
  }

  bool found_all_queue_families =
      found_graphics && found_compute && found_transfer && found_present;
  YERR_IF(!found_all_queue_families);

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos(
      queue_families_to_create.size(), VkDeviceQueueCreateInfo{});
  std::vector<float> queue_priorities(queue_families_to_create.size(), 1.f);

  size_t i = 0;
  for (uint32_t family_index : queue_families_to_create) {
    queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos[i].queueFamilyIndex = family_index;
    queue_create_infos[i].queueCount = 1;
    queue_create_infos[i].pQueuePriorities = &queue_priorities[i];
  }

  VkDeviceCreateInfo device_create_info = {};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.queueCreateInfoCount = queue_create_infos.size();

  VkPhysicalDeviceFeatures features = {};
  device_create_info.pEnabledFeatures = &features;

  std::vector<const char*> required_extensions = GetRequiredExtensions();
  device_create_info.enabledExtensionCount =
      static_cast<uint32_t>(required_extensions.size());
  device_create_info.ppEnabledExtensionNames = required_extensions.data();
  device_create_info.enabledLayerCount = 0;

  VkDevice logical_device;
  YERR_IF(vkCreateDevice(physical_device, &device_create_info, nullptr,
                         &logical_device) != VK_SUCCESS);

  vkGetDeviceQueue(logical_device, graphics_queue->family_index,
                   /*queue_index = */ 0, &graphics_queue->handle);
  vkGetDeviceQueue(logical_device, compute_queue->family_index,
                   /*queue_index = */ 0, &compute_queue->handle);
  vkGetDeviceQueue(logical_device, transfer_queue->family_index,
                   /*queue_index = */ 0, &transfer_queue->handle);
  vkGetDeviceQueue(logical_device, present_queue->family_index,
                   /*queue_index = */ 0, &present_queue->handle);

  return logical_device;
}

}  // namespace

VulkanDevice::VulkanDevice(const VulkanInstance& instance,
                           const VulkanSurface& surface)
    : physical_device_(
          SelectPhysicalDevice(instance.handle(), surface.handle())) {
  logical_device_ = CreateLogicalDeviceAndQueues(
      physical_device_, surface.handle(), &graphics_queue_, &compute_queue_,
      &transfer_queue_, &present_queue_);
}

}  // namespace y
