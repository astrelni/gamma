// Copyright (c) 2018 Aleksey Strelnikov
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

#ifndef GAMMA_GRAPHICS_VK_SHADER_MODULE_HPP_
#define GAMMA_GRAPHICS_VK_SHADER_MODULE_HPP_

#include <cstdint>
#include <vector>

#include "gamma/graphics/vk/device.hpp"

namespace y {

class VulkanShaderModule {
 public:
  VulkanShaderModule() = default;
  VulkanShaderModule(const VulkanShaderModule&) = delete;
  VulkanShaderModule(VulkanShaderModule&& x) noexcept;
  VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;
  VulkanShaderModule& operator=(VulkanShaderModule&& x) noexcept;
  ~VulkanShaderModule();

  VulkanShaderModule(const VulkanDevice& device,
                     const std::vector<uint32_t>& spv_byte_code);

  explicit operator bool() const;
  VkShaderModule handle() const;

 private:
  void Reset();

  VkDevice device_ = VK_NULL_HANDLE;
  VkShaderModule module_ = VK_NULL_HANDLE;
};

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

VulkanShaderModule::VulkanShaderModule(VulkanShaderModule&& x) noexcept
    : device_(x.device_), module_(x.module_) {
  x.device_ = VK_NULL_HANDLE;
  x.module_ = VK_NULL_HANDLE;
}

VulkanShaderModule& VulkanShaderModule::operator=(
    VulkanShaderModule&& x) noexcept {
  Reset();
  device_ = x.device_;
  module_ = x.module_;
  x.device_ = VK_NULL_HANDLE;
  x.module_ = VK_NULL_HANDLE;
  return *this;
}

inline VulkanShaderModule::~VulkanShaderModule() { Reset(); }

inline VulkanShaderModule::operator bool() const {
  return module_ != VK_NULL_HANDLE;
}

inline VkShaderModule VulkanShaderModule::handle() const { return module_; }

inline void VulkanShaderModule::Reset() {
  if (*this) vkDestroyShaderModule(device_, module_, nullptr);
}

}  // namespace y
#endif  // GAMMA_GRAPHICS_VK_SHADER_MODULE_HPP_
