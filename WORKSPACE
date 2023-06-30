load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Check minimum bazel version.
http_archive(
    name = "bazel_skylib",
    sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
    ],
)

load("@bazel_skylib//lib:versions.bzl", "versions")

versions.check(minimum_bazel_version = "5.0.0")

# Foreign rules tool.
http_archive(
    name = "rules_foreign_cc",
    sha256 = "6041f1374ff32ba711564374ad8e007aef77f71561a7ce784123b9b4b88614fc",
    strip_prefix = "rules_foreign_cc-0.8.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.8.0.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://github.com/bazelbuild/rules_foreign_cc/tree/main/docs#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

http_archive(
    name = "com_google_absl",
    sha256 = "6764f226bd6e2d8ab9fe2f3cab5f45fb1a4a15c04b58b87ba7fa87456054f98b",
    strip_prefix = "abseil-cpp-273292d1cfc0a94a65082ee350509af1d113344d",
    urls = ["https://github.com/abseil/abseil-cpp/archive/273292d1cfc0a94a65082ee350509af1d113344d.zip"],
)

http_archive(
    name = "glfw",
    build_file = "@//:BUILD.glfw",
    sha256 = "fd21a5f65bcc0fc3c76e0f8865776e852de09ef6fbc3620e09ce96d2b2807e04",
    strip_prefix = "glfw-3.3.7",
    urls = ["https://github.com/glfw/glfw/archive/3.3.7.tar.gz"],
)

# proto_library rules implicitly depend on @com_google_protobuf//:protoc,
# which is the proto-compiler.
# This statement defines the @com_google_protobuf repo.
http_archive(
    name = "com_google_protobuf",
    urls = ["https://github.com/google/protobuf/archive/b4b0e304be5a68de3d0ee1af9b286f958750f5e4.zip"],
)

# cc_proto_library rules implicitly depend on @com_google_protobuf_cc//:cc_toolchain,
# which is the C++ proto runtime (base classes and common utilities).
http_archive(
    name = "com_google_protobuf_cc",
    urls = ["https://github.com/google/protobuf/archive/b4b0e304be5a68de3d0ee1af9b286f958750f5e4.zip"],
)

# GoogleTest for C++ testing.
http_archive(
    name = "com_google_googletest",
    sha256 = "199e68f9dff997b30d420bf23cd9a0d3f66bfee4460e2cd95084a2c45ee00f1a",
    strip_prefix = "googletest-5376968f6948923e2411081fd9372e71a59d8e77",
    urls = ["https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip"],
)
