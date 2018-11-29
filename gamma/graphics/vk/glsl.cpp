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

#include "gamma/graphics/vk/glsl.hpp"

#include "SPIRV/GlslangToSpv.h"
#include "gamma/common/log.hpp"
#include "glslang/Include/ResourceLimits.h"

namespace y {
namespace {

TBuiltInResource DefaultResource() {
  return {/* .MaxLights = */ 32,
          /* .MaxClipPlanes = */ 6,
          /* .MaxTextureUnits = */ 32,
          /* .MaxTextureCoords = */ 32,
          /* .MaxVertexAttribs = */ 64,
          /* .MaxVertexUniformComponents = */ 4096,
          /* .MaxVaryingFloats = */ 64,
          /* .MaxVertexTextureImageUnits = */ 32,
          /* .MaxCombinedTextureImageUnits = */ 80,
          /* .MaxTextureImageUnits = */ 32,
          /* .MaxFragmentUniformComponents = */ 4096,
          /* .MaxDrawBuffers = */ 32,
          /* .MaxVertexUniformVectors = */ 128,
          /* .MaxVaryingVectors = */ 8,
          /* .MaxFragmentUniformVectors = */ 16,
          /* .MaxVertexOutputVectors = */ 16,
          /* .MaxFragmentInputVectors = */ 15,
          /* .MinProgramTexelOffset = */ -8,
          /* .MaxProgramTexelOffset = */ 7,
          /* .MaxClipDistances = */ 8,
          /* .MaxComputeWorkGroupCountX = */ 65535,
          /* .MaxComputeWorkGroupCountY = */ 65535,
          /* .MaxComputeWorkGroupCountZ = */ 65535,
          /* .MaxComputeWorkGroupSizeX = */ 1024,
          /* .MaxComputeWorkGroupSizeY = */ 1024,
          /* .MaxComputeWorkGroupSizeZ = */ 64,
          /* .MaxComputeUniformComponents = */ 1024,
          /* .MaxComputeTextureImageUnits = */ 16,
          /* .MaxComputeImageUniforms = */ 8,
          /* .MaxComputeAtomicCounters = */ 8,
          /* .MaxComputeAtomicCounterBuffers = */ 1,
          /* .MaxVaryingComponents = */ 60,
          /* .MaxVertexOutputComponents = */ 64,
          /* .MaxGeometryInputComponents = */ 64,
          /* .MaxGeometryOutputComponents = */ 128,
          /* .MaxFragmentInputComponents = */ 128,
          /* .MaxImageUnits = */ 8,
          /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
          /* .MaxCombinedShaderOutputResources = */ 8,
          /* .MaxImageSamples = */ 0,
          /* .MaxVertexImageUniforms = */ 0,
          /* .MaxTessControlImageUniforms = */ 0,
          /* .MaxTessEvaluationImageUniforms = */ 0,
          /* .MaxGeometryImageUniforms = */ 0,
          /* .MaxFragmentImageUniforms = */ 8,
          /* .MaxCombinedImageUniforms = */ 8,
          /* .MaxGeometryTextureImageUnits = */ 16,
          /* .MaxGeometryOutputVertices = */ 256,
          /* .MaxGeometryTotalOutputComponents = */ 1024,
          /* .MaxGeometryUniformComponents = */ 1024,
          /* .MaxGeometryVaryingComponents = */ 64,
          /* .MaxTessControlInputComponents = */ 128,
          /* .MaxTessControlOutputComponents = */ 128,
          /* .MaxTessControlTextureImageUnits = */ 16,
          /* .MaxTessControlUniformComponents = */ 1024,
          /* .MaxTessControlTotalOutputComponents = */ 4096,
          /* .MaxTessEvaluationInputComponents = */ 128,
          /* .MaxTessEvaluationOutputComponents = */ 128,
          /* .MaxTessEvaluationTextureImageUnits = */ 16,
          /* .MaxTessEvaluationUniformComponents = */ 1024,
          /* .MaxTessPatchComponents = */ 120,
          /* .MaxPatchVertices = */ 32,
          /* .MaxTessGenLevel = */ 64,
          /* .MaxViewports = */ 16,
          /* .MaxVertexAtomicCounters = */ 0,
          /* .MaxTessControlAtomicCounters = */ 0,
          /* .MaxTessEvaluationAtomicCounters = */ 0,
          /* .MaxGeometryAtomicCounters = */ 0,
          /* .MaxFragmentAtomicCounters = */ 8,
          /* .MaxCombinedAtomicCounters = */ 8,
          /* .MaxAtomicCounterBindings = */ 1,
          /* .MaxVertexAtomicCounterBuffers = */ 0,
          /* .MaxTessControlAtomicCounterBuffers = */ 0,
          /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
          /* .MaxGeometryAtomicCounterBuffers = */ 0,
          /* .MaxFragmentAtomicCounterBuffers = */ 1,
          /* .MaxCombinedAtomicCounterBuffers = */ 1,
          /* .MaxAtomicCounterBufferSize = */ 16384,
          /* .MaxTransformFeedbackBuffers = */ 4,
          /* .MaxTransformFeedbackInterleavedComponents = */ 64,
          /* .MaxCullDistances = */ 8,
          /* .MaxCombinedClipAndCullDistances = */ 8,
          /* .MaxSamples = */ 4,
          /* .maxMeshOutputVerticesNV = */ 256,
          /* .maxMeshOutputPrimitivesNV = */ 512,
          /* .maxMeshWorkGroupSizeX_NV = */ 32,
          /* .maxMeshWorkGroupSizeY_NV = */ 1,
          /* .maxMeshWorkGroupSizeZ_NV = */ 1,
          /* .maxTaskWorkGroupSizeX_NV = */ 32,
          /* .maxTaskWorkGroupSizeY_NV = */ 1,
          /* .maxTaskWorkGroupSizeZ_NV = */ 1,
          /* .maxMeshViewCountNV = */ 4,

          /* .limits = */
          {
              /* .nonInductiveForLoops = */ 1,
              /* .whileLoops = */ 1,
              /* .doWhileLoops = */ 1,
              /* .generalUniformIndexing = */ 1,
              /* .generalAttributeMatrixVectorIndexing = */ 1,
              /* .generalVaryingIndexing = */ 1,
              /* .generalSamplerIndexing = */ 1,
              /* .generalVariableIndexing = */ 1,
              /* .generalConstantMatrixVectorIndexing = */ 1,
          }};
}

}  // namespace

std::vector<uint32_t> CompileGLSL(EShLanguage stage, absl::string_view source) {
  static const TBuiltInResource resource = DefaultResource();

  glslang::TShader shader(stage);

  const char* source_data = source.data();
  int source_size = static_cast<int>(source.size());
  shader.setStringsWithLengths(&source_data, &source_size, 1);

  YERR_IF(!shader.parse(&resource, /* defaultVersion = */ 100,
                        /* forwardCompatible = */ false, EShMsgDefault))
      << shader.getInfoLog();

  std::vector<uint32_t> spirv;
  glslang::GlslangToSpv(*shader.getIntermediate(), spirv);
  return spirv;
}

}  // namespace y
