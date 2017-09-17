new_http_archive(
    name = "glfw",
    build_file = "BUILD.glfw",
    sha256 = "e10f0de1384d75e6fc210c53e91843f6110d6c4f3afbfb588130713c2f9d8fe8",
    strip_prefix = "glfw-3.2.1",
    urls = ["https://github.com/glfw/glfw/archive/3.2.1.tar.gz"],
)

new_http_archive(
    name = "assimp",
    build_file = "BUILD.assimp",
    sha256 = "60080d8ab4daaab309f65b3cffd99f19eb1af8d05623fff469b9b652818e286e",
    strip_prefix = "assimp-4.0.1",
    urls = ["https://github.com/assimp/assimp/archive/v4.0.1.tar.gz"],
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
