# Copyright (c) 2018-2019 Aleksey Strelnikov
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

workspace(name = "astrelni_gamma")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# External dependencies:

# GoogleTest
http_archive(
     name = "com_google_googletest",
     sha256 = "927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7",
     urls = ["https://github.com/google/googletest/archive/release-1.8.1.zip"],
     strip_prefix = "googletest-release-1.8.1",
)

# Abseil
http_archive(
    name = "com_google_absl",
    sha256 = "9f545d8a072d47d80429afa467706376ffa2f3e809edec3fc1e7114c29dcb42c",
    urls = ["https://github.com/abseil/abseil-cpp/archive/66f9becbb98ecc083f4db349b4b1e0ca9de93b15.zip"],
    strip_prefix = "abseil-cpp-66f9becbb98ecc083f4db349b4b1e0ca9de93b15",
)

# Protobuf
http_archive(
    name = "com_google_protobuf",
    sha256 = "678d91d8a939a1ef9cb268e1f20c14cd55e40361dc397bb5881e4e1e532679b1",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/v3.10.1.zip"],
    strip_prefix = "protobuf-3.10.1",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

# GLFW
http_archive(
    name = "glfw",
    sha256 = "e10f0de1384d75e6fc210c53e91843f6110d6c4f3afbfb588130713c2f9d8fe8",
    urls = ["https://github.com/glfw/glfw/archive/3.2.1.tar.gz"],
    strip_prefix = "glfw-3.2.1",
    build_file_content = """
config_setting(
    name = "linux",
    constraint_values = [
        "@bazel_tools//platforms:linux",
    ],
)

cc_library(
    name = "glfw",
    visibility = ["//visibility:public"],
    hdrs = ["include/GLFW/glfw3.h"],
    srcs = [
        # include glfw3.h in srcs as well because bazel is having trouble with
        # relative include paths inside glfw source.
        "include/GLFW/glfw3.h",
        "src/context.c",
        "src/init.c",
        "src/input.c",
        "src/internal.h",
        "src/monitor.c",
        "src/vulkan.c",
        "src/window.c",
    ] + select({
        ":linux": [
            "src/egl_context.c",
            "src/egl_context.h",
            "src/glx_context.c",
            "src/glx_context.h",
            "src/linux_joystick.c",
            "src/linux_joystick.h",
            "src/posix_time.c",
            "src/posix_time.h",
            "src/posix_tls.c",
            "src/posix_tls.h",
            "src/x11_init.c",
            "src/x11_monitor.c",
            "src/x11_platform.h",
            "src/x11_window.c",
            "src/xkb_unicode.c",
            "src/xkb_unicode.h",
        ],
    }),
    defines = select({
        ":linux": ["_GLFW_X11"],
    }),
    strip_include_prefix = "include",
    linkopts = select({
        ":linux": [
            "-ldl",
            "-lX11",
            "-lXrandr",
            "-lXinerama",
            "-lXcursor",
            "-lXxf86vm",
            "-lpthread",
            "-lvulkan",
        ],
    }),
)
""",
)

# GLM
http_archive(
    name = "glm",
    sha256 = "81ae3005eda9352921c77f5c5e5cd6bb5644abfea0808c535328ec4f989b7e86",
    urls = ["https://github.com/g-truc/glm/archive/0.9.9.3.zip"],
    strip_prefix = "glm-0.9.9.3",
    build_file_content = """
cc_library(
    name = "glm_impl",
    hdrs = glob(["glm/**/*.hpp"], exclude = ["glm/glm.hpp"]) +
        glob(["glm/**/*.h"]) + glob(["glm/**/*.inl"]),
)

cc_library(
    name = "glm",
    visibility = ["//visibility:public"],
    hdrs = ["glm/glm.hpp"],
    deps = [":glm_impl"],
)
""",
)

# glslang
http_archive(
  name = "glslang",
  sha256 = "1dc6baa9394518d1cabc3a38cf7e5ecaaafe23447d04f29d7a2f5c667ea89024",
  urls = ["https://github.com/KhronosGroup/glslang/archive/7.10.2984.zip"],
  strip_prefix = "glslang-7.10.2984",
  build_file_content = """
config_setting(
    name = "linux",
    constraint_values = [
        "@bazel_tools//platforms:linux",
    ],
)

cc_library(
    name = "glslang",
    visibility = ["//visibility:public"],
    hdrs = ["glslang/Public/ShaderLang.h"],
    srcs =
        glob(["glslang/Include/*.h"]) +
        glob(["glslang/MachineIndependent/*.cpp"]) +
        glob(["glslang/MachineIndependent/*.h"]) +
        glob(["glslang/MachineIndependent/preprocessor/*.cpp"]) +
        glob(["glslang/MachineIndependent/preprocessor/*.h"]) +
        [
            "glslang/OSDependent/osinclude.h",
            "glslang/GenericCodeGen/CodeGen.cpp",
            "glslang/GenericCodeGen/Link.cpp",
            "OGLCompilersDLL/InitializeDll.h",
            "OGLCompilersDLL/InitializeDll.cpp",
        ] +
        select({":linux": ["glslang/OSDependent/Unix/ossource.cpp"]}),
)

cc_library(
    name = "spirv",
    visibility = ["//visibility:public"],
    hdrs = [
        "SPIRV/bitutils.h",
        "SPIRV/spirv.hpp",
        "SPIRV/GLSL.std.450.h",
        "SPIRV/GLSL.ext.EXT.h",
        "SPIRV/GLSL.ext.KHR.h",
        "SPIRV/GlslangToSpv.h",
        "SPIRV/hex_float.h",
        "SPIRV/Logger.h",
        "SPIRV/SpvBuilder.h",
        "SPIRV/spvIR.h",
        "SPIRV/doc.h",
        "SPIRV/SpvTools.h",
        "SPIRV/disassemble.h",
    ],
    srcs = [
        "SPIRV/GlslangToSpv.cpp",
        "SPIRV/InReadableOrder.cpp",
        "SPIRV/Logger.cpp",
        "SPIRV/SpvBuilder.cpp",
        "SPIRV/SpvPostProcess.cpp",
        "SPIRV/doc.cpp",
        "SPIRV/SpvTools.cpp",
        "SPIRV/disassemble.cpp",
    ],
    deps = [":glslang"],
)
""",
)

# spir-v headers
http_archive(
    name = "spirv_headers",
    sha256 = "7b00d0fd6f21985e387bd9c536ac612f7a8ba5741ec118cd35a7ccdd76bf34b8",
    urls = ["https://github.com/KhronosGroup/SPIRV-Headers/archive/1.5.1.zip"],
    strip_prefix = "SPIRV-Headers-1.5.1",
    build_file_content = """
cc_library(
    name = "spirv_headers",
    visibility = ["//visibility:public"],
    includes = ["include"],
    hdrs = [
        "include/spirv/unified1/GLSL.std.450.h",
        "include/spirv/unified1/OpenCL.std.h",
        "include/spirv/unified1/spirv.h",
    ],
)

cc_library(
    name = "spirv_headers_1_2",
    visibility = ["//visibility:public"],
    includes = ["include"],
    hdrs = [
        "include/spirv/1.2/GLSL.std.450.h",
    ],
)

filegroup(
    name = "core_grammar",
    visibility = ["//visibility:public"],
    srcs = ["include/spirv/unified1/spirv.core.grammar.json"],
)

filegroup(
    name = "glsl_grammar",
    visibility = ["//visibility:public"],
    srcs = ["include/spirv/unified1/extinst.glsl.std.450.grammar.json"],
)

filegroup(
    name = "opencl_grammar",
    visibility = ["//visibility:public"],
    srcs = ["include/spirv/unified1/extinst.opencl.std.100.grammar.json"],
)

filegroup(
    name = "spv_xml",
    visibility = ["//visibility:public"],
    srcs = ["include/spirv/spir-v.xml"],
)
"""
)

# spir-v tools
http_archive(
    name = "spirv_tools",
    sha256 = "2502b01744dd9bbc82182b96ff394ca105574d90a7da041e9274164807bab018",
    urls = ["https://github.com/KhronosGroup/SPIRV-Tools/archive/v2019.4.zip"],
    strip_prefix = "SPIRV-Tools-2019.4",
    build_file_content = """
filegroup(
    name = "debug_info_grammar",
    srcs = ["source/extinst.debuginfo.grammar.json"],
)

genrule(
    name = "core_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = [
        "core.insts-unified1.inc",
        "operand.kinds-unified1.inc",
    ],
    cmd = "python $< --spirv-core-grammar=$(location @spirv_headers//:core_grammar)  --extinst-debuginfo-grammar=$(location :debug_info_grammar) --core-insts-output=$(RULEDIR)/core.insts-unified1.inc --operand-kinds-output=$(RULEDIR)/operand.kinds-unified1.inc",
    tools = [
        ":debug_info_grammar",
        "@spirv_headers//:core_grammar",
    ],
)

genrule(
    name = "enum_string_mapping",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = [
        "extension_enum.inc",
        "enum_string_mapping.inc",
    ],
    cmd = "python $< --spirv-core-grammar=$(location @spirv_headers//:core_grammar) --extinst-debuginfo-grammar=$(location :debug_info_grammar) --extension-enum-output=$(RULEDIR)/extension_enum.inc --enum-string-mapping-output=$(RULEDIR)/enum_string_mapping.inc",
    tools = [
        ":debug_info_grammar",
        "@spirv_headers//:core_grammar",
    ],
)

genrule(
    name = "debug_info_h",
    srcs = ["utils/generate_language_headers.py"],
    outs = ["DebugInfo.h"],
    cmd = "python $< --extinst-name=DebugInfo --extinst-grammar=$(location :debug_info_grammar) --extinst-output-base=$(RULEDIR)/DebugInfo",
    tools = [
        ":debug_info_grammar",
    ],
)

genrule(
    name = "glsl_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = ["glsl.std.450.insts.inc"],
    cmd = "python $< --extinst-glsl-grammar=$(location @spirv_headers//:glsl_grammar) --glsl-insts-output=$(OUTS)",
    tools = ["@spirv_headers//:glsl_grammar"],
)

genrule(
    name = "opencl_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = ["opencl.std.insts.inc"],
    cmd = "python $< --extinst-opencl-grammar=$(location @spirv_headers//:opencl_grammar) --opencl-insts-output=$(OUTS)",
    tools = ["@spirv_headers//:opencl_grammar"],
)

filegroup(
    name = "amd_shader_explicit_vertex_parameter_grammar",
    srcs = ["source/extinst.spv-amd-shader-explicit-vertex-parameter.grammar.json"],
)

genrule(
    name = "amd_shader_explicit_vertex_parameter_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = ["spv-amd-shader-explicit-vertex-parameter.insts.inc"],
    cmd = "python $< --extinst-vendor-grammar=$(location :amd_shader_explicit_vertex_parameter_grammar) --vendor-insts-output=$(OUTS)",
    tools = [":amd_shader_explicit_vertex_parameter_grammar"],
)

filegroup(
    name = "amd_shader_trinary_minmax_grammar",
    srcs = ["source/extinst.spv-amd-shader-trinary-minmax.grammar.json"],
)

genrule(
    name = "amd_shader_trinary_minmax_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = ["spv-amd-shader-trinary-minmax.insts.inc"],
    cmd = "python $< --extinst-vendor-grammar=$(location :amd_shader_trinary_minmax_grammar) --vendor-insts-output=$(OUTS)",
    tools = [":amd_shader_trinary_minmax_grammar"],
)

filegroup(
    name = "amd_gcn_shader_grammar",
    srcs = ["source/extinst.spv-amd-gcn-shader.grammar.json"],
)

genrule(
    name = "amd_gcn_shader_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = ["spv-amd-gcn-shader.insts.inc"],
    cmd = "python $< --extinst-vendor-grammar=$(location :amd_gcn_shader_grammar) --vendor-insts-output=$(OUTS)",
    tools = [":amd_gcn_shader_grammar"],
)

filegroup(
    name = "amd_shader_ballot_grammar",
    srcs = ["source/extinst.spv-amd-shader-ballot.grammar.json"],
)

genrule(
    name = "amd_shader_ballot_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = ["spv-amd-shader-ballot.insts.inc"],
    cmd = "python $< --extinst-vendor-grammar=$(location :amd_shader_ballot_grammar) --vendor-insts-output=$(OUTS)",
    tools = [":amd_shader_ballot_grammar"],
)

genrule(
    name = "debug_info_tables",
    srcs = ["utils/generate_grammar_tables.py"],
    outs = ["debuginfo.insts.inc"],
    cmd = "python $< --extinst-vendor-grammar=$(location :debug_info_grammar) --vendor-insts-output=$(OUTS)",
    tools = [
        ":debug_info_grammar",
    ],
)

genrule(
    name = "generators_inc",
    srcs = ["utils/generate_registry_tables.py"],
    outs = ["generators.inc"],
    cmd = "python $< --xml=$(location @spirv_headers//:spv_xml) --generator-output=$(OUTS)",
    tools = ["@spirv_headers//:spv_xml"],
)

filegroup(
    name = "changes",
    srcs = ["CHANGES"],
)

genrule(
    name = "build_version_inc",
    srcs = ["utils/update_build_version.py"],
    outs = ["build-version.inc"],
    cmd = "python $< external/spirv_tools $(OUTS)",
    tools = [":changes"],
)

cc_library(
    name = "spirv_tools",
    visibility = ["//visibility:public"],
    includes = ["include", "."],
    hdrs = [
        "include/spirv-tools/instrument.hpp",
        "include/spirv-tools/libspirv.h",
        "include/spirv-tools/libspirv.hpp",
        "include/spirv-tools/linker.hpp",
        "include/spirv-tools/optimizer.hpp",
    ],
    srcs =
        glob(["source/*.h"]) +
        glob(["source/*.cpp"]) +
        # glob(["source/fuzz/*.h"]) +
        # glob(["source/fuzz/*.cpp"]) +
        glob(["source/link/*.h"]) +
        glob(["source/link/*.cpp"]) +
        glob(["source/opt/*.h"]) +
        glob(["source/opt/*.cpp"]) +
        # glob(["source/reduce/*.h"]) +
        # glob(["source/reduce/*.cpp"]) +
        glob(["source/util/*.h"]) +
        glob(["source/util/*.cpp"]) +
        glob(["source/val/*.h"]) +
        glob(["source/val/*.cpp"]) +
        [
            ":build-version.inc",
            ":core.insts-unified1.inc",
            ":DebugInfo.h",
            ":debuginfo.insts.inc",
            ":enum_string_mapping.inc",
            ":extension_enum.inc",
            ":generators.inc",
            ":glsl.std.450.insts.inc",
            ":opencl.std.insts.inc",
            ":operand.kinds-unified1.inc",
            ":spv-amd-shader-explicit-vertex-parameter.insts.inc",
            ":spv-amd-shader-trinary-minmax.insts.inc",
            ":spv-amd-gcn-shader.insts.inc",
            ":spv-amd-shader-ballot.insts.inc",
        ],
    deps = [
        "@spirv_headers//:spirv_headers",
        "@spirv_headers//:spirv_headers_1_2",
    ],
)
"""
)

# [
#     "source/util/bitutils.h",
#     "source/util/bit_vector.h",
#     "source/util/hex_float.h",
#     "source/util/make_unique.h",
#     "source/util/parse_number.h",
#     "source/util/small_vector.h",
#     "source/util/string_utils.h",
#     "source/util/timer.h",
#     "source/assembly_grammar.h",
#     "source/binary.h",
#     "source/cfa.h",
#     "source/diagnostic.h",
#     "source/disassemble.h",
#     "source/enum_set.h",
#     "source/enum_string_mapping.h",
#     "source/ext_inst.h",
#     "source/extensions.h",
#     "source/instruction.h",
#     "source/latest_version_glsl_std_450_header.h",
#     "source/latest_version_opencl_std_header.h",
#     "source/latest_version_spirv_header.h",
#     "source/macro.h",
#     "source/name_mapper.h",
#     "source/opcode.h",
#     "source/operand.h",
#     "source/parsed_operand.h",
#     "source/print.h",
#     "source/spirv_constant.h",
#     "source/spirv_definition.h",
#     "source/spirv_endian.h",
#     "source/spirv_fuzzer_options.h",
#     "source/spirv_optimizer_options.h",
#     "source/spirv_reducer_options.h",
#     "source/spirv_target_env.h",
#     "source/spirv_validator_options.h",
#     "source/table.h",
#     "source/text.h",
#     "source/text_handler.h",
#     "source/util/bit_vector.cpp",
#     "source/util/parse_number.cpp",
#     "source/util/string_utils.cpp",
#     "source/assembly_grammar.cpp",
#     "source/binary.cpp",
#     "source/diagnostic.cpp",
#     "source/disassemble.cpp",
#     "source/enum_string_mapping.cpp",
#     "source/ext_inst.cpp",
#     "source/extensions.cpp",
#     "source/libspirv.cpp",
#     "source/name_mapper.cpp",
#     "source/opcode.cpp",
#     "source/operand.cpp",
#     "source/parsed_operand.cpp",
#     "source/print.cpp",
#     "source/software_version.cpp",
#     "source/spirv_endian.cpp",
#     "source/spirv_fuzzer_options.cpp",
#     "source/spirv_optimizer_options.cpp",
#     "source/spirv_reducer_options.cpp",
#     "source/spirv_target_env.cpp",
#     "source/spirv_validator_options.cpp",
#     "source/table.cpp",
#     "source/text.cpp",
#     "source/text_handler.cpp",
#     "source/val/validate.cpp",
#     "source/val/validate.h",
#     "source/val/validate_adjacency.cpp",
#     "source/val/validate_annotation.cpp",
#     "source/val/validate_arithmetics.cpp",
#     "source/val/validate_atomics.cpp",
#     "source/val/validate_barriers.cpp",
#     "source/val/validate_bitwise.cpp",
#     "source/val/validate_builtins.cpp",
#     "source/val/validate_capability.cpp",
#     "source/val/validate_cfg.cpp",
#     "source/val/validate_composites.cpp",
#     "source/val/validate_constants.cpp",
#     "source/val/validate_conversion.cpp",
#     "source/val/validate_debug.cpp",
#     "source/val/validate_decorations.cpp",
#     "source/val/validate_derivatives.cpp",
#     "source/val/validate_extensions.cpp",
#     "source/val/validate_execution_limitations.cpp",
#     "source/val/validate_function.cpp",
#     "source/val/validate_id.cpp",
#     "source/val/validate_image.cpp",
#     "source/val/validate_interfaces.cpp",
#     "source/val/validate_instruction.cpp",
#     "source/val/validate_layout.cpp",
#     "source/val/validate_literals.cpp",
#     "source/val/validate_logicals.cpp",
#     "source/val/validate_memory.cpp",
#     "source/val/validate_memory_semantics.cpp",
#     "source/val/validate_memory_semantics.h",
#     "source/val/validate_misc.cpp",
#     "source/val/validate_mode_setting.cpp",
#     "source/val/validate_non_uniform.cpp",
#     "source/val/validate_primitives.cpp",
#     "source/val/validate_scopes.cpp",
#     "source/val/validate_scopes.h",
#     "source/val/validate_small_type_uses.cpp",
#     "source/val/validate_type.cpp",
#     "source/val/decoration.h",
#     "source/val/basic_block.cpp",
#     "source/val/basic_block.h",
#     "source/val/construct.cpp",
#     "source/val/construct.h",
#     "source/val/function.cpp",
#     "source/val/function.h",
#     "source/val/instruction.cpp",
#     "source/val/instruction.h",
#     "source/val/validation_state.cpp",
#     "source/val/validation_state.h",

# shaderc
http_archive(
    name = "shaderc",
    sha256 = "dbcf82215747b3633a024a09cc5cfb43ad407eb1d3d500636e7bd16b483da05d",
    urls = ["https://github.com/google/shaderc/archive/v2019.0.zip"],
    strip_prefix = "shaderc-2019.0",
    build_file_content = """
cc_library(
    name = "shaderc_util",
    includes = ["libshaderc_util/include"],
    hdrs = [
        "libshaderc_util/include/libshaderc_util/compiler.h",
        "libshaderc_util/include/libshaderc_util/counting_includer.h",
        "libshaderc_util/include/libshaderc_util/exceptions.h",
        "libshaderc_util/include/libshaderc_util/file_finder.h",
        "libshaderc_util/include/libshaderc_util/format.h",
        "libshaderc_util/include/libshaderc_util/io.h",
        "libshaderc_util/include/libshaderc_util/message.h",
        "libshaderc_util/include/libshaderc_util/mutex.h",
        "libshaderc_util/include/libshaderc_util/resources.h",
        "libshaderc_util/include/libshaderc_util/shader_stage.h",
        "libshaderc_util/include/libshaderc_util/spirv_tools_wrapper.h",
        "libshaderc_util/include/libshaderc_util/string_piece.h",
        "libshaderc_util/include/libshaderc_util/universal_unistd.h",
        "libshaderc_util/include/libshaderc_util/version_profile.h",
    ],
    srcs = [
        "libshaderc_util/include/libshaderc_util/resources.inc",
        "libshaderc_util/src/compiler.cc",
        "libshaderc_util/src/file_finder.cc",
        "libshaderc_util/src/io.cc",
        "libshaderc_util/src/resources.cc",
        "libshaderc_util/src/message.cc",
        "libshaderc_util/src/shader_stage.cc",
        "libshaderc_util/src/spirv_tools_wrapper.cc",
        "libshaderc_util/src/version_profile.cc",
    ],
    deps = [
        "@glslang//:glslang",
        "@glslang//:spirv",
        "@spirv_tools//:spirv_tools",
    ],
)

cc_library(
    name = "shaderc",
    visibility = ["//visibility:public"],
    includes = ["libshaderc/include"],
    hdrs = ["libshaderc/include/shaderc/shaderc.hpp"],
    srcs = [
        "libshaderc/include/shaderc/env.h",
        "libshaderc/include/shaderc/shaderc.h",
        "libshaderc/include/shaderc/status.h",
        "libshaderc/include/shaderc/visibility.h",
        "libshaderc/src/shaderc.cc",
        "libshaderc/src/shaderc_private.h",
    ],
    deps = [
        ":shaderc_util",
    ],
)
"""
)

# LuaJIT
http_archive(
    name = "luajit",
    sha256 = "2adbe397a5b6b8ab22fa8396507ce852a2495db50e50734b3daa1ffcadd9eeb4",
    urls = ["https://github.com/LuaJIT/LuaJIT/archive/v2.0.5.zip"],
    strip_prefix = "LuaJIT-2.0.5",
    build_file_content = """
genrule(
    name = "luajit_ar",
    srcs =
        [
            "Makefile",
            "src/Makefile",
            "src/Makefile.dep",
            "src/lj.supp",
            "src/host/genminilua.lua",
        ] +
        glob(["src/*.h"]) +
        glob(["src/*.c"]) +
        glob(["src/*.dasc"]) +
        glob(["src/host/*.h"]) +
        glob(["src/host/*.c"]) +
        glob(["src/jit/*.lua"]) +
        glob(["dynasm/*.h"]) +
        glob(["dynasm/*.lua"]),
    outs = ["libluajit.a"],
    cmd = "make -C external/luajit && cp external/luajit/src/libluajit.a $(OUTS)",
)

cc_library(
    name = "luajit",
    visibility = ["//visibility:public"],
    srcs = glob(["src/*.h"]) + ["libluajit.a"],
    hdrs = ["src/lua.hpp"],
    includes = ["src"],
    linkopts = ["-ldl"],
)
"""
)

# Bullet Physics

# Probably others...
