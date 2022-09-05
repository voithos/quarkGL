# Common linker options for OpenGL programs on Linux.
# Pass to linkopts in cc_binary.
LINUX_LINKOPTS = [
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
