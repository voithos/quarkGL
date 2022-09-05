package(default_visibility = ["//visibility:public"])

cc_library(
    name = "quarkgl",
    hdrs = ["quarkgl.h"],
    include_prefix = "qrk",
    deps = [
        ":camera",
        ":core",
        ":exceptions",
        ":framebuffer",
        ":light",
        ":mesh",
        ":mesh_primitives",
        ":model",
        ":screen",
        ":shader",
        ":shader_compiler",
        ":shader_defs",
        ":shader_loader",
        ":shader_primitives",
        ":shadows",
        ":texture",
        ":texture_map",
        ":utils",
        ":vertex_array",
        ":window",
        "//third_party/glad",
        "@glfw",
    ],
)

cc_library(
    name = "core",
    srcs = ["core.cc"],
    hdrs = ["core.h"],
    include_prefix = "qrk",
    deps = [
        "//third_party/glad",
        "@glfw",
    ],
)

cc_library(
    name = "camera",
    srcs = ["camera.cc"],
    hdrs = ["camera.h"],
    include_prefix = "qrk",
    deps = [
        ":core",
        ":exceptions",
        ":light",
        ":screen",
        ":shader",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)

cc_library(
    name = "exceptions",
    srcs = ["exceptions.cc"],
    hdrs = ["exceptions.h"],
    include_prefix = "qrk",
    deps = [
        "//third_party/glad",
    ],
)

cc_library(
    name = "light",
    srcs = ["light.cc"],
    hdrs = ["light.h"],
    include_prefix = "qrk",
    deps = [
        ":exceptions",
        ":shader",
        "//third_party/glm",
    ],
)

cc_library(
    name = "mesh",
    srcs = ["mesh.cc"],
    hdrs = ["mesh.h"],
    include_prefix = "qrk",
    deps = [
        ":shader",
        ":texture_map",
        ":vertex_array",
        "//third_party/glad",
        "//third_party/glm",
    ],
)

cc_library(
    name = "mesh_primitives",
    srcs = ["mesh_primitives.cc"],
    hdrs = ["mesh_primitives.h"],
    include_prefix = "qrk",
    deps = [
        ":mesh",
        ":texture",
        ":texture_map",
    ],
)

cc_library(
    name = "texture",
    srcs = ["texture.cc"],
    hdrs = ["texture.h"],
    include_prefix = "qrk",
    deps = [
        ":exceptions",
        "//third_party/glad",
        "//third_party/glm",
        "//third_party/stb_image",
    ],
)

cc_library(
    name = "texture_map",
    hdrs = ["texture_map.h"],
    include_prefix = "qrk",
    deps = [
        ":exceptions",
        ":texture",
        "//third_party/glad",
        "@assimp",
    ],
)

cc_library(
    name = "framebuffer",
    srcs = ["framebuffer.cc"],
    hdrs = ["framebuffer.h"],
    include_prefix = "qrk",
    deps = [
        ":screen",
        ":window",
        "//third_party/glad",
        "//third_party/glm",
    ],
)

cc_library(
    name = "model",
    srcs = ["model.cc"],
    hdrs = ["model.h"],
    include_prefix = "qrk",
    deps = [
        ":exceptions",
        ":mesh",
        ":shader",
        ":texture",
        ":texture_map",
        "//third_party/glad",
        "//third_party/glm",
        "//third_party/stb_image",
        "@assimp",
    ],
)

cc_library(
    name = "screen",
    hdrs = ["screen.h"],
    include_prefix = "qrk",
)

cc_library(
    name = "shader",
    srcs = ["shader.cc"],
    hdrs = ["shader.h"],
    include_prefix = "qrk",
    deps = [
        ":core",
        ":exceptions",
        ":shader_compiler",
        ":shader_defs",
        ":shader_loader",
        ":texture",
        "//third_party/glad",
        "//third_party/glm",
    ],
)

cc_library(
    name = "shader_primitives",
    srcs = ["shader_primitives.cc"],
    hdrs = ["shader_primitives.h"],
    include_prefix = "qrk",
    deps = [":shader"],
)

cc_library(
    name = "shader_defs",
    hdrs = ["shader_defs.h"],
    include_prefix = "qrk",
)

cc_library(
    name = "shader_loader",
    srcs = ["shader_loader.cc"],
    hdrs = ["shader_loader.h"],
    data = glob([
        "shaders/**/*.glsl",
        "shaders/**/*.vert",
        "shaders/**/*.frag",
        "shaders/**/*.geom",
    ]),
    include_prefix = "qrk",
    deps = [
        ":exceptions",
        ":shader_defs",
        ":utils",
    ],
)

cc_library(
    name = "shader_compiler",
    srcs = ["shader_compiler.cc"],
    hdrs = ["shader_compiler.h"],
    include_prefix = "qrk",
    deps = [
        ":exceptions",
        ":shader_defs",
        ":shader_loader",
    ],
)

cc_library(
    name = "shadows",
    srcs = ["shadows.cc"],
    hdrs = ["shadows.h"],
    include_prefix = "qrk",
    deps = [
        ":exceptions",
        ":light",
        ":shader",
        "//third_party/glm",
    ],
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
    deps = [
        "//third_party/glad",
    ],
)

cc_library(
    name = "window",
    srcs = ["window.cc"],
    hdrs = ["window.h"],
    include_prefix = "qrk",
    deps = [
        ":camera",
        ":core",
        ":exceptions",
        ":screen",
        ":shader",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)
