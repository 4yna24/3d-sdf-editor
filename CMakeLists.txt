# SDF Editor: top level build.
# Each folder in src/ is one module and one library. See ai/code-structure.md.

cmake_minimum_required(VERSION 3.24)
project(sdf_editor LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SDFE_BUILD_TESTS "Build the module tests" ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(sdfe_module)   # sdfe_add_module(), sdfe_add_tests()

if(SDFE_BUILD_TESTS)
  enable_testing()
endif()

# Outside libraries: SDL3, glad, Dear ImGui, GLM, nlohmann/json, doctest.
add_subdirectory(extern)

# Modules, from the bottom layer to the top.
add_subdirectory(src/base)
add_subdirectory(src/platform)
add_subdirectory(src/gpu)
add_subdirectory(src/props)
add_subdirectory(src/sdf)
add_subdirectory(src/prefs)
add_subdirectory(src/scene)
add_subdirectory(src/ui)
add_subdirectory(src/io)
add_subdirectory(src/draw)
add_subdirectory(src/wm)
add_subdirectory(src/editors)
add_subdirectory(src/app)

# The layer rules are checked with the tests.
if(SDFE_BUILD_TESTS)
  find_package(Python3 COMPONENTS Interpreter)
  if(Python3_FOUND)
    add_test(NAME check_layers
             COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/check_layers.py)
  endif()
endif()
