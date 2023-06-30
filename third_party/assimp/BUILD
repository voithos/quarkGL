load("@//build_defs:expand_template.bzl", "expand_template")

# Statically compiled library. This ended up being far simpler
# and less error-prone than trying to reproduce assimp's build
# system in bazel, especially since it relies on tons of inconsistent
# include paths.
cc_import(
    name = "assimp_lib",
    hdrs = glob([
        "include/assimp/**/*.h",
        "include/assimp/**/*.inl",
        "include/assimp/**/*.hpp",
    ]),
    static_library = "assimp-vc143-mtd.lib",
)

cc_import(
    name = "zlib_lib",
    static_library = "zlibstaticd.lib",
    alwayslink = True,
)

expand_template(
    name = "config_tmpl",
    out = "include/assimp/config.h",
    substitutions = {"#cmakedefine": "// #undef"},
    template = "include/assimp/config.h.in",
)

cc_library(
    name = "assimp",
    hdrs = glob([
        "include/assimp/**/*.h",
        "include/assimp/**/*.inl",
        "include/assimp/**/*.hpp",
    ]) + [":config_tmpl"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        ":assimp_lib",
        ":zlib_lib",
    ],
)
