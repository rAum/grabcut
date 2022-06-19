cmake_minimum_required(VERSION 3.17)
project(grabcut LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    message(WARNING "No build type explicitly set. Set to Release")
    set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "RelWithDebInfo" "Release" "MinSizeRel" )
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

if (TRUE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
endif()

add_compile_options( "-Werror" "-Wall" "-Wpedantic" "-Wextra")

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory(grabcut)
add_subdirectory(apps EXCLUDE_FROM_ALL)



