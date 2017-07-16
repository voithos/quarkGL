cc_library(
    name = "glad",
    srcs = ["glad.c"],
    hdrs = [
        "glad.h",
        "khrplatform.h",
    ],
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
        ":glad",
        "@glfw//:glfw",
    ],
)
