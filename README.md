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

# Legal stuff

Uses [SFML](https://github.com/SFML/SFML), which is licensed under the
[SFML license](https://github.com/SFML/SFML/blob/master/license.md).

Uses [imgui](https://github.com/ocornut/imgui), which is licensed under the
[MIT](https://choosealicense.com/licenses/mit/) license.

Uses [imgui-sfml](https://github.com/eliasdaler/imgui-sfml), which is licensed
under the [MIT](https://choosealicense.com/licenses/mit/) license.

Uses [catch](https://github.com/catchorg/Catch2), which is licensed under the
[Boost Software](https://choosealicense.com/licenses/bsl-1.0/) license.
