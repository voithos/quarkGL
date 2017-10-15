# TODO: Expose a single :quarkgl library and make the rest private.

cc_library(
    name = "core",
    srcs = ["core.cc"],
    hdrs = ["core.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/glad",
        "@glfw//:glfw",
    ],
)

cc_library(
    name = "camera",
    srcs = ["camera.cc"],
    hdrs = ["camera.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/glad",
        "//third_party/glm",
    ],
)

cc_library(
    name = "light",
    srcs = ["light.cc"],
    hdrs = ["light.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        ":shader",
        ":shared",
        "//third_party/glm",
    ],
)

cc_library(
    name = "mesh",
    srcs = ["mesh.cc"],
    hdrs = ["mesh.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        ":shader",
        ":vertex_array",
        "//third_party/glad",
        "//third_party/glm",
    ],
)

cc_library(
    name = "model",
    srcs = ["model.cc"],
    hdrs = ["model.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        ":mesh",
        ":shader",
        ":shared",
        "//third_party/glad",
        "//third_party/glm",
        "//third_party/stb_image",
        "@assimp//:assimp",
    ],
)

cc_library(
    name = "shader",
    srcs = ["shader.cc"],
    hdrs = ["shader.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":shader_loader",
        ":shared",
        "//third_party/glad",
        "//third_party/glm",
    ],
)

cc_library(
    name = "shader_loader",
    srcs = ["shader_loader.cc"],
    hdrs = ["shader_loader.h"],
    data = glob([
        "shaders/*.vert",
        "shaders/*.frag",
    ]),
    include_prefix = "qrk",
    deps = [
        ":shared",
        ":utils",
    ],
)

cc_library(
    name = "shared",
    hdrs = ["shared.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [],
)

cc_library(
    name = "utils",
    hdrs = ["utils.h"],
    include_prefix = "qrk",
    deps = [],
)

cc_library(
    name = "vertex_array",
    srcs = ["vertex_array.cc"],
    hdrs = ["vertex_array.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/glad",
    ],
)

cc_library(
    name = "window",
    srcs = ["window.cc"],
    hdrs = ["window.h"],
    include_prefix = "qrk",
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":shared",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw//:glfw",
    ],
)
