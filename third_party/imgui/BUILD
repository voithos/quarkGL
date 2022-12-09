licenses(["notice"])

cc_library(
    name = "imgui",
    srcs = glob([
        "*.cpp",
        "backends/*.cpp",
    ]),
    hdrs = glob([
        "*.h",
        "backends/*.h",
    ]),
    includes = [
        ".",
        "backends",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/glm",
        "@glfw",
    ],
)
