DEFINES = [
    "_GLFW_X11",
    "_GLFW_HAS_XF86VM",
]

cc_library(
    name = "includes",
    hdrs = glob([
        "include/GLFW/*.h",
        "src/*.h",
    ]),
)

cc_library(
    name = "context",
    srcs = ["src/context.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "init",
    srcs = ["src/init.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "input",
    srcs = ["src/input.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "monitor",
    srcs = ["src/monitor.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "vulkan",
    srcs = ["src/vulkan.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "window",
    srcs = ["src/window.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "x11_init",
    srcs = ["src/x11_init.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "x11_monitor",
    srcs = ["src/x11_monitor.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "x11_window",
    srcs = ["src/x11_window.c"],
    defines = DEFINES,
    deps = [
        ":egl_context",
        ":glx_context",
        ":includes",
    ],
)

cc_library(
    name = "xkb_unicode",
    srcs = ["src/xkb_unicode.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "linux_joystick",
    srcs = ["src/linux_joystick.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "posix_time",
    srcs = ["src/posix_time.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "posix_tls",
    srcs = ["src/posix_tls.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "glx_context",
    srcs = ["src/glx_context.c"],
    defines = DEFINES,
    deps = [
        ":context",
        ":includes",
        ":posix_tls",
    ],
)

cc_library(
    name = "egl_context",
    srcs = ["src/egl_context.c"],
    defines = DEFINES,
    deps = [":includes"],
)

cc_library(
    name = "glfw",
    hdrs = glob(["include/GLFW/*.h"]),
    defines = DEFINES,
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        ":context",
        ":egl_context",
        ":glx_context",
        ":init",
        ":input",
        ":linux_joystick",
        ":monitor",
        ":posix_time",
        ":posix_tls",
        ":vulkan",
        ":window",
        ":x11_init",
        ":x11_monitor",
        ":x11_window",
        ":xkb_unicode",
    ],
)
