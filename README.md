# Triangles

THIS PROJECT IS STILL A WORK IN PROGRESS!

A gui application that lets you draw triangles of different colors on a colored
background.

Uses [imgui](https://github.com/ocornut/imgui) for UI.

# Compiling

Create a build dir, run cmake, and then make to build.

    mkdir buildDebug
    cd buildDebug
    cmake ..
    make

## Dependencies

SFML should be installed on the system. imgui, and imgui-sfml are git submodules
and should be initialized with `git submodule update --init --recursive`.
