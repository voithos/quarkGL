# Common linker options for OpenGL programs.
# Pass to linkopts in cc_binary.
OPENGL_LINKOPTS = select({
    "@platforms//os:linux": [
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
    ],
    "@platforms//os:windows": [
        "-DEFAULTLIB:user32.lib",
        "-DEFAULTLIB:gdi32.lib",
        "-DEFAULTLIB:shell32.lib",
        "-DEFAULTLIB:opengl32.lib",
        "-DEFAULTLIB:glu32.lib",
        "-NODEFAULTLIB:ucrt.lib",
        "-NODEFAULTLIB:vcruntime.lib",
        "-NODEFAULTLIB:msvcrt.lib",
        "-DEFAULTLIB:kernel32.lib",
        "-DEFAULTLIB:advapi32.lib",
    ],
    "//conditions:default": [],
})
