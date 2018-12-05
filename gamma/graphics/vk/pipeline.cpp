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

#include "gamma/graphics/vk/pipeline.hpp"

namespace y {}

// void createGraphicsPipeline() {
//   auto vertShaderCode = readFile("shaders/vert.spv");
//   auto fragShaderCode = readFile("shaders/frag.spv");
//
//   VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
//   VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
//
//   VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
//   vertShaderStageInfo.sType =
//       VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//   vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//   vertShaderStageInfo.module = vertShaderModule;
//   vertShaderStageInfo.pName = "main";
//
//   VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
//   fragShaderStageInfo.sType =
//       VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//   fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//   fragShaderStageInfo.module = fragShaderModule;
//   fragShaderStageInfo.pName = "main";
//
//   VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
//                                                     fragShaderStageInfo};
//
//   vkDestroyShaderModule(device, fragShaderModule, nullptr);
//   vkDestroyShaderModule(device, vertShaderModule, nullptr);
// }
