cc_library(
    name = "shader",
    srcs = ["shader.cc"],
    hdrs = ["shader.h"],
    deps = ["//third_party:glad"],
)

cc_binary(
    name = "learn_opengl",
    srcs = ["learn_opengl.cc"],
    data = [
        "fragment.glsl",
        "vertex.glsl",
    ],
    linkopts = [
        "-lGL",
        "-lGLU",
        "-lX11",
        "-lXxf86vm",
        "-lXinerama",
        "-lXcursor",
        "-lpthread",
        "-lXrandr",
        "-lXi",
        "-ldl",
    ],
    deps = [
        ":shader",
        "//third_party:glad",
        "@glfw//:glfw",
    ],
)
