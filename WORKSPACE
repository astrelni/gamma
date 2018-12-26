workspace(name = "astrelni_gamma")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# External dependencies:

# GoogleTest
http_archive(
     name = "com_google_googletest",
     urls = ["https://github.com/google/googletest/archive/release-1.8.1.zip"],
     strip_prefix = "googletest-release-1.8.1",
)

# Abseil
http_archive(
    name = "com_google_absl",
    urls = ["https://github.com/abseil/abseil-cpp/archive/7990fd459e9339467814ddb95000c87cb1e4d945.zip"],
    strip_prefix = "abseil-cpp-7990fd459e9339467814ddb95000c87cb1e4d945",
)

# Protobuf
http_archive(
    name = "com_google_protobuf",
    urls = ["https://github.com/protocolbuffers/protobuf/releases/download/v3.6.1/protobuf-cpp-3.6.1.zip"],
    strip_prefix = "protobuf-3.6.1",
)

# GLFW
http_archive(
    name = "glfw",
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

# Vulkan

# LuaJIT

# Bullet Physics

# Probably others...
