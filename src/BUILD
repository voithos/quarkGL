cc_library(
    name = "camera",
    srcs = ["camera.cc"],
    hdrs = ["camera.h"],
    include_prefix = "qrk",
    deps = [
        "//third_party/glad",
        "//third_party/glm",
    ],
)

cc_library(
    name = "shader",
    srcs = ["shader.cc"],
    hdrs = ["shader.h"],
    include_prefix = "qrk",
    deps = [
        "//third_party/glad",
        "//third_party/glm",
    ],
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
        ":camera",
        ":shader",
        "//third_party/glad",
        "//third_party/glm",
        "//third_party/stb_image",
        "@glfw//:glfw",
    ],
)
