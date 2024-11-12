cmake_minimum_required(VERSION 3.16)
# extend module path with local module/ directory
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})

include(version)
project(grabcut VERSION ${ver} LANGUAGES CXX)

include(ccache)
include(setup_project)
include(test_utils)
if (BUILD_TESTS)
    # required to enable ctest support
    enable_testing()
endif()

option(DEV_USE_CCACHE "If enabled, tries to find ccache program and use it to speedup recompilation" ON)
option(BUILD_GRABCUT_APPS "If enabled, builds frontend apps - it requires OpenCV!" ON)
option(BUILD_PYGRABCUT "If enabled, build python module. Ensure you checkout pybind11 submodule." ON)

add_subdirectory(external_deps)
add_subdirectory(grabcut)

if (BUILD_GRABCUT_APPS)
    add_subdirectory(apps)
endif()

if (BUILD_PYGRABCUT)
    message("Building python module is enabled. If you get pybind11 related errors, run \" git submodule update --init\"")
    add_subdirectory(python)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

