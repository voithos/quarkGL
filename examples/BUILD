# TODO: Pull these into a common location.
QRK_LINKOPTS = [
    "-lGL",
    "-lGLU",
    "-lX11",
    "-lXxf86vm",
    # "-lXinerama",
    "-lXcursor",
    "-lpthread",
    "-lXrandr",
    "-lXi",
    "-ldl",
]

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
    linkopts = QRK_LINKOPTS,
    deps = [
        "//quarkgl:camera",
        "//quarkgl:core",
        "//quarkgl:light",
        "//quarkgl:mesh_primitives",
        "//quarkgl:model",
        "//quarkgl:shader",
        "//quarkgl:shader_primitives",
        "//quarkgl:vertex_array",
        "//quarkgl:window",
        "//third_party/glad",
        "//third_party/glm",
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
    linkopts = QRK_LINKOPTS,
    deps = [
        "//quarkgl:camera",
        "//quarkgl:core",
        "//quarkgl:light",
        "//quarkgl:model",
        "//quarkgl:shader",
        "//quarkgl:vertex_array",
        "//quarkgl:window",
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
    linkopts = QRK_LINKOPTS,
    deps = [
        "//quarkgl:camera",
        "//quarkgl:core",
        "//quarkgl:light",
        "//quarkgl:mesh_primitives",
        "//quarkgl:model",
        "//quarkgl:shader",
        "//quarkgl:vertex_array",
        "//quarkgl:window",
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
    linkopts = QRK_LINKOPTS,
    deps = [
        "//quarkgl:camera",
        "//quarkgl:core",
        "//quarkgl:framebuffer",
        "//quarkgl:shader",
        "//quarkgl:texture",
        "//quarkgl:vertex_array",
        "//quarkgl:window",
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
    linkopts = QRK_LINKOPTS,
    deps = [
        "//quarkgl:core",
        "//quarkgl:mesh_primitives",
        "//quarkgl:shader",
        "//quarkgl:shader_primitives",
        "//quarkgl:texture",
        "//quarkgl:window",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)

cc_binary(
    name = "shadow_map",
    srcs = ["shadow_map.cc"],
    data = [
        "assets/wood.png",
        ":shaders",
    ],
    linkopts = QRK_LINKOPTS,
    deps = [
        "//quarkgl:core",
        "//quarkgl:framebuffer",
        "//quarkgl:mesh_primitives",
        "//quarkgl:shader",
        "//quarkgl:shader_primitives",
        "//quarkgl:shadows",
        "//quarkgl:texture",
        "//quarkgl:texture_map",
        "//quarkgl:window",
        "//third_party/glad",
        "//third_party/glm",
        "@glfw",
    ],
)
