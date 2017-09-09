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

## Developing

In addition to the build tooling, you may also want to build a [compilation
database](http://clang.llvm.org/docs/JSONCompilationDatabase.html) in order to
enable semantic completion in your editor. The database is processed through
some Python tooling that requires the `google.protobuf` library in order to
work properly.

To install `google.protobuf`, first [install
`pip`](https://pip.pypa.io/en/stable/installing/), the Python package
installer, and then run:

    $ sudo pip install protobuf

Afterwards, execute the following script to generate the compilation database:

    $ ./build_compile_commands.sh

## Acknowledgements

This library was built as a way to learn modern OpenGL and graphics techniques.
A big thank you goes to the wonderful tutorials by Joey de Vries at
[learnopengl.com](https://learnopengl.com)!
