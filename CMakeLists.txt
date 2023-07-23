cmake_minimum_required(VERSION 3.8.2)
project(Triangles LANGUAGES CXX VERSION 1.0)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm/glm/glm.hpp)
    message(FATAL_ERROR "third_party/glm/glm/glm.hpp is missing!\n \
Please update the glm submodule by running 'git submodule init' and \
'git submodule update'!")
endif()
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/raygui/src/raygui.h)
    message(FATAL_ERROR "third_party/raygui/src/raygui.h is missing!\n \
Please update the raygui submodule by running 'git submodule init' and \
'git submodule update'!")
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(Triangles_MAIN_SOURCES
    src/main.cpp
)

set(Triangles_LIB_SOURCES
    src/state.cpp
    src/shape.cpp
    src/triangle.cpp
    src/circle.cpp
    src/raygui.cpp
)

add_library(TrianglesLib STATIC ${Triangles_LIB_SOURCES})

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_executable(Triangles WIN32 ${Triangles_MAIN_SOURCES})
else()
    add_executable(Triangles ${Triangles_MAIN_SOURCES})
endif()

target_compile_options(TrianglesLib PRIVATE
    -Wall -Wextra -Wpedantic
    -Wsuggest-override
    $<$<COMPILE_LANGUAGE:CXX>:-Weffc++>
    $<$<CONFIG:DEBUG>:-O0>
)
target_compile_options(Triangles PRIVATE
    -Wall -Wextra -Wpedantic
    -Wsuggest-override
    $<$<COMPILE_LANGUAGE:CXX>:-Weffc++>
    $<$<CONFIG:DEBUG>:-O0>
)

target_link_libraries(Triangles PUBLIC TrianglesLib)

target_compile_features(TrianglesLib PUBLIC cxx_std_17)

if(BUILD_SHARED_LIBS OR (UNIX AND NOT CYGWIN) OR (CMAKE_CXX_COMPILER_ID MATCHES "MSVC"))
    find_package(raylib 4.2 REQUIRED)
else()
    find_package(raylib 4.2 REQUIRED)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
target_link_libraries(TrianglesLib PUBLIC
    raylib
    opengl32
)
else()
target_link_libraries(TrianglesLib PUBLIC
    raylib
    GL
)
endif()

target_include_directories(TrianglesLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/raygui/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(Triangles_UNIT_TEST_SOURCES
        src/unittest/test_helpers.cpp
        third_party/catch/catch_amalgamated.cpp
    )

    add_executable(UnitTest_Triangles ${Triangles_UNIT_TEST_SOURCES})
    target_link_libraries(UnitTest_Triangles TrianglesLib)
    target_include_directories(UnitTest_Triangles PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch
    )
endif()
