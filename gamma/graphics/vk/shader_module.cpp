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

#include "gamma/graphics/vk/shader_module.hpp"

namespace y {

VulkanShaderModule::VulkanShaderModule(VkDevice logical_device,
                                       VkShaderStageFlagBits shader_stage,
                                       absl::Span<const uint32_t> spv_byte_code)
    : device_(logical_device), stage_(shader_stage) {
  YERR_IF(spv_byte_code.empty());
  VkShaderModuleCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = spv_byte_code.size() * sizeof(uint32_t);
  create_info.pCode = spv_byte_code.data();
  YERR_IF(vkCreateShaderModule(device_, &create_info, nullptr, &module_) !=
          VK_SUCCESS);
}

VkPipelineShaderStageCreateInfo VulkanShaderModule::makeCreateInfo() const {
  VkPipelineShaderStageCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  info.module = module_;
  info.pName = "main";
  return info;
}

}  // namespace y
