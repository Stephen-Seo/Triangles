cmake_minimum_required(VERSION 3.0)
project(Triangles LANGUAGES CXX VERSION 1.0)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
    message(FATAL_ERROR "third_party/imgui is missing!\nPlease update the \
GameDevTools submodule by running 'git submodule init' and 'git submodule \
update'!")
endif()
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-sfml)
    message(FATAL_ERROR "third_party/imgui-sfml is missing!\nPlease update the \
GameDevTools submodule by running 'git submodule init' and 'git submodule \
update'!")
endif()

set(ImGuiDemo "")
if((NOT CMAKE_BUILD_TYPE) OR (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(ImGuiDemo "third_party/imgui/imgui_demo.cpp")
endif()

set(Triangles_SOURCES
    src/main.cpp
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui-sfml/imgui-SFML.cpp
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wsuggest-override")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -D NDEBUG")

add_executable(Triangles ${Triangles_SOURCES})

target_compile_features(Triangles PUBLIC cxx_std_17)

if(BUILD_SHARED_LIBS OR (UNIX AND NOT CYGWIN))
    find_package(SFML 2 REQUIRED
        COMPONENTS audio network graphics window system)
else()
    find_package(SFML 2 REQUIRED
        COMPONENTS audio-s network-s graphics-s window-s system-s)
    add_definitions(-DSFML_STATIC)
endif()

target_link_libraries(Triangles
    sfml-graphics sfml-window sfml-system
    GL
)

target_include_directories(Triangles PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SFML_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include # imgui related headers
)
# Use macro to override imgui config header
target_compile_definitions(Triangles PRIVATE
    "IMGUI_USER_CONFIG=\"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-sfml/imconfig-SFML.h\"")
