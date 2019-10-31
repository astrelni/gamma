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

#include "gamma/graphics/vk/glsl.hpp"

#include <stdio.h>

#include "shaderc/shaderc.hpp"

#include "absl/container/fixed_array.h"
#include "absl/memory/memory.h"
#include "absl/strings/match.h"
#include "gamma/common/log.hpp"

namespace y {
namespace {

shaderc_shader_kind ToShadercKind(VkShaderStageFlagBits stage) {
  if (stage == VK_SHADER_STAGE_VERTEX_BIT) return shaderc_vertex_shader;
  if (stage == VK_SHADER_STAGE_FRAGMENT_BIT) return shaderc_fragment_shader;
  if (stage == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
    return shaderc_tess_control_shader;
  if (stage == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
    return shaderc_tess_evaluation_shader;
  if (stage == VK_SHADER_STAGE_GEOMETRY_BIT) return shaderc_geometry_shader;
  YERR_IF(stage != VK_SHADER_STAGE_COMPUTE_BIT) << "unsupported shader stage";
  return shaderc_compute_shader;
}

void CheckCompilationResult(const shaderc::SpvCompilationResult& result) {
  shaderc_compilation_status status = result.GetCompilationStatus();
  if (status == shaderc_compilation_status_success) return;

  std::string error_message = result.GetErrorMessage();
  YERR_IF(status == shaderc_compilation_status_invalid_stage) << error_message;
  YERR_IF(status == shaderc_compilation_status_compilation_error)
      << error_message;
  YERR_IF(status == shaderc_compilation_status_internal_error) << error_message;
  YERR_IF(status == shaderc_compilation_status_null_result_object)
      << error_message;
  YERR_IF(status == shaderc_compilation_status_invalid_assembly)
      << error_message;
  YERR_IF(status == shaderc_compilation_status_validation_error)
      << error_message;
  YERR_IF(status == shaderc_compilation_status_transformation_error)
      << error_message;
}

std::vector<uint32_t> TranslateGLSLToSPIRV(shaderc_shader_kind kind,
                                           const std::string& source_name,
                                           absl::string_view source) {
  shaderc::CompileOptions options;
  options.SetOptimizationLevel(shaderc_optimization_level_performance);
  options.SetTargetEnvironment(shaderc_target_env_vulkan, 0);

  shaderc::Compiler compiler;
  shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
      source.data(), source.size(), kind, source_name.c_str(), options);
  CheckCompilationResult(result);
  return std::vector<uint32_t>(result.begin(), result.end());
}

absl::FixedArray<char> ReadFile(const std::string& path) {
  FILE* file = fopen(path.c_str(), "r");
  YERR_IF(file == nullptr) << "failed to open '" << path << "'";

  size_t nchars = 0;
  while (fgetc(file) != EOF) ++nchars;

  fseek(file, 0, SEEK_SET);

  absl::FixedArray<char> buffer(nchars + 1);
  YERR_IF(fread(buffer.data(), sizeof(char), nchars, file) != nchars);
  fclose(file);

  buffer[nchars] = '\0';
  return buffer;
}

VkShaderStageFlagBits GetShaderStageFromFilePath(absl::string_view path) {
  if (absl::EndsWith(path, ".vert")) return VK_SHADER_STAGE_VERTEX_BIT;
  if (absl::EndsWith(path, ".frag")) return VK_SHADER_STAGE_FRAGMENT_BIT;
  if (absl::EndsWith(path, ".tesc"))
    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
  if (absl::EndsWith(path, ".tese"))
    return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
  if (absl::EndsWith(path, ".geom")) return VK_SHADER_STAGE_GEOMETRY_BIT;
  if (!absl::EndsWith(path, ".comp")) {
    YERR << "unsupported glsl file '" << path
         << "'; must end in one of {.vert, .frag, .tesc, .tese, .geom, .comp}";
  }
  return VK_SHADER_STAGE_COMPUTE_BIT;
}

}  // namespace

VulkanShaderModule MakeVulkanShaderFromGLSLSource(
    VkDevice logical_device, const std::string& source_name,
    absl::string_view source) {
  VkShaderStageFlagBits stage = GetShaderStageFromFilePath(source_name);
  std::vector<uint32_t> byte_code =
      TranslateGLSLToSPIRV(ToShadercKind(stage), source_name, source);
  return VulkanShaderModule(logical_device, stage, byte_code);
}

VulkanShaderModule MakeVulkanShaderFromGLSLFile(VkDevice logical_device,
                                                const std::string& path) {
  absl::FixedArray<char> file_data = ReadFile(path);
  return MakeVulkanShaderFromGLSLSource(
      logical_device, path,
      absl::string_view(file_data.data(), file_data.size()));
}

}  // namespace y
