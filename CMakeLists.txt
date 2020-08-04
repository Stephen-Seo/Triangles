cmake_minimum_required(VERSION 3.0)
project(Triangles LANGUAGES CXX VERSION 1.0)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.h)
    message(FATAL_ERROR "third_party/imgui/imgui.h is missing!\n \
Please update the GameDevTools submodule by running 'git submodule init' and \
'git submodule update'!")
endif()
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-sfml/imgui-SFML.h)
    message(FATAL_ERROR "third_party/imgui-sfml/imgui-SFML.h is missing!\n \
Please update the GameDevTools submodule by running 'git submodule init' and \
'git submodule update'!")
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(ImGuiDemo "third_party/imgui/imgui_demo.cpp")
else()
    set(ImGuiDemo "")
endif()

set(Triangles_MAIN_SOURCES
    src/main.cpp
)

set(Triangles_LIB_SOURCES
    src/state.cpp
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui-sfml/imgui-SFML.cpp
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wsuggest-override")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -D NDEBUG")

add_library(TrianglesLib STATIC ${Triangles_LIB_SOURCES})

add_executable(Triangles ${Triangles_MAIN_SOURCES})

target_link_libraries(Triangles PUBLIC TrianglesLib)

target_compile_features(TrianglesLib PUBLIC cxx_std_17)

if(BUILD_SHARED_LIBS OR (UNIX AND NOT CYGWIN))
    find_package(SFML 2 REQUIRED
        COMPONENTS audio network graphics window system)
else()
    find_package(SFML 2 REQUIRED
        COMPONENTS audio-s network-s graphics-s window-s system-s)
    add_definitions(-DSFML_STATIC)
endif()

target_link_libraries(TrianglesLib PUBLIC
    sfml-graphics sfml-window sfml-system
    GL
)

target_include_directories(TrianglesLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SFML_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/build_include # imgui related headers
)
# Use macro to override imgui config header
target_compile_definitions(TrianglesLib PRIVATE
    "IMGUI_USER_CONFIG=\"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-sfml/imconfig-SFML.h\"")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(Triangles_UNIT_TEST_SOURCES
        src/unittest/test_main.cpp
        src/unittest/test_helpers.cpp
    )

    add_executable(UnitTest_Triangles ${Triangles_UNIT_TEST_SOURCES})
    target_link_libraries(UnitTest_Triangles TrianglesLib)
    target_include_directories(UnitTest_Triangles PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch
    )
endif()
