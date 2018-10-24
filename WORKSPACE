workspace(name = "astrelni_gamma")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# External dependencies:

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
        ":linux": [
            "_GLFW_VULKAN_STATIC",
            "_GLFW_X11",
        ],
    }),
    strip_include_prefix = "include",
)
""",
)

# Vulkan

# LuaJIT

# Bullet Physics

# Abseil

# Probably others...
