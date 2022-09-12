load("//quarkgl:quarkgl.bzl", "LINUX_LINKOPTS")

filegroup(
    name = "shaders",
    srcs = glob(["shaders/*"]),
)

cc_binary(
    name = "model_render",
    srcs = ["model_render.cc"],
    data = [
        ":shaders",
        "assets/container.jpg",
    ] + glob([
        "assets/nanosuit/*",
        "assets/skybox/*",
    ]),
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@glfw",
    ],
)

cc_binary(
    name = "instancing",
    srcs = ["instancing.cc"],
    data = [
        ":shaders",
    ] + glob([
        "assets/rock/*",
        "assets/planet/*",
    ]),
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)

cc_binary(
    name = "geom_shader",
    srcs = ["geom_shader.cc"],
    data = [
        ":shaders",
    ] + glob(["assets/nanosuit/*"]),
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)

cc_binary(
    name = "post_processing",
    srcs = ["post_processing.cc"],
    data = [
        "assets/container.jpg",
        "assets/metal.png",
    ],
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)

cc_binary(
    name = "compute",
    srcs = ["compute.cc"],
    data = [
        ":shaders",
    ],
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glm",
    ],
)

cc_binary(
    name = "shadow_map",
    srcs = ["shadow_map.cc"],
    data = [
        "assets/wood.png",
        ":shaders",
    ],
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)

cc_binary(
    name = "normal_map",
    srcs = ["normal_map.cc"],
    data = [
        "assets/brickwall.jpg",
        "assets/brickwall_normal.jpg",
        ":shaders",
    ],
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)
