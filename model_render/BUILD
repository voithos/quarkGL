load("//quarkgl:quarkgl.bzl", "LINUX_LINKOPTS")

cc_binary(
    name = "model_render",
    srcs = ["model_render.cc"],
    data = [
        "//examples:assets",
    ] + glob([
        "shaders/**",
    ]),
    linkopts = LINUX_LINKOPTS,
    deps = [
        "//quarkgl",
        "//third_party/glad",
        "//third_party/glm",
        "//third_party/imgui",
        "//third_party/imguizmo_quat",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@glfw",
    ],
)
