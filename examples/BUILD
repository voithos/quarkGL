# TODO: Pull these into a common location.
QRK_LINKOPTS = [
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
]

cc_binary(
    name = "model_render",
    srcs = ["model_render.cc"],
    data = [
        "phong.frag",
        "model.vert",
        "model_normals.geom",
    ] + glob(["nanosuit/*"]),
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
        "@glfw//:glfw",
    ],
)

cc_binary(
    name = "geom_shader",
    srcs = ["geom_shader.cc"],
    data = [
        "phong.frag",
        "model.vert",
        "explode.geom",
    ] + glob(["nanosuit/*"]),
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
        "@glfw//:glfw",
    ],
)

cc_binary(
    name = "post_processing",
    srcs = ["post_processing.cc"],
    data = [
        "metal.png",
        "container.jpg",
    ] + glob(["nanosuit/*"]),
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
        "@glfw//:glfw",
    ],
)
