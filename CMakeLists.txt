cmake_minimum_required(VERSION 3.17)
project(grabcut VERSION 0.0.0 LANGUAGES CXX)

# extend module path with local module/ directory
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})

include(ccache)
include(setup_project)

add_compile_options( "-Werror" "-Wall" "-Wpedantic" "-Wextra")

option(DEV_USE_CCACHE "If enabled, tries to find ccache program and use it to speedup recompilation" ON)
find_package(OpenCV 4 REQUIRED core video imgproc objdetect dnn highgui)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory(grabcut)
add_subdirectory(apps EXCLUDE_FROM_ALL)



