# This is wrapping of maxflow library from https://pub.ist.ac.at/~vnk/software.html
# and a code from my uni student project. I think it's under GPL
# TODO: Update code and clean this up

add_library(maxflow SHARED
    graph.cpp
    maxflow.cpp
)

target_include_directories(maxflow PUBLIC SYSTEM
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
)
set_target_properties(maxflow PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_STANDARD 11)
