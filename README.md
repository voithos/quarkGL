quarkGL
=======

A subatomic OpenGL graphics library.

## Building

quarkGL uses [Bazel](https://bazel.build/) as its build system. To begin, first
[install Bazel](https://docs.bazel.build/install.html).

To be able to actually compile the dependencies, run the following to install
the appropriate tools and headers (on Ubuntu/Debian):

    $ sudo apt-get install build-essential xorg-dev libgl1-mesa-dev libglu1-mesa-dev

Then build the examples, and run them:

    $ bazel build examples:all
    $ bazel run examples:learn_opengl
