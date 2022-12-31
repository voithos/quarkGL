load("//quarkgl:quarkgl.bzl", "LINUX_LINKOPTS")

filegroup(
    name = "shaders",
    srcs = glob(["shaders/*"]),
)

filegroup(
    name = "assets",
    srcs = glob(["assets/**"]),
    visibility = ["//model_render:__pkg__"],
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

cc_binary(
    name = "hdr",
    srcs = ["hdr.cc"],
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
    name = "bloom",
    srcs = ["bloom.cc"],
    data = [
        "assets/container2.png",
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
    name = "deferred",
    srcs = ["deferred.cc"],
    data = [
        ":shaders",
    ] + glob([
        "assets/DamagedHelmet/*",
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
    name = "ssao",
    srcs = ["ssao.cc"],
    data = [
        "assets/brickwall.jpg",
        "assets/brickwall_normal.jpg",
        ":shaders",
    ] + glob([
        "assets/SurvivalGuitarBackpack/**",
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
    name = "pbr",
    srcs = ["pbr.cc"],
    data = [
        "assets/wood.png",
        ":shaders",
    ] + glob([
        "assets/rusted_iron/*",
    ]),
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)
