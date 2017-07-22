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
        "awesomeface.png",
        "container.jpg",
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
        "//third_party:stb_image",
        "//third_party/glm",
        "@glfw//:glfw",
    ],
)
