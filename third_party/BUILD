cc_library(
    name = "glad",
    srcs = ["glad.c"],
    hdrs = ["glad.h"],
    include_prefix = "glad",
    visibility = ["//visibility:public"],
    deps = [":khr"],
)

cc_library(
    name = "khr",
    hdrs = ["khrplatform.h"],
    include_prefix = "KHR",
    visibility = ["//visibility:public"],
)

cc_library(
    name = "stb_image",
    srcs = ["stb_image.cc"],
    hdrs = ["stb_image.h"],
    include_prefix = "stb",
    visibility = ["//visibility:public"],
)
