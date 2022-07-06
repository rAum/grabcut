# This library is on GPL licence and is updated version from
# https://github.com/gerddie/maxflow
# where original is from  http://pub.ist.ac.at/~vnk/software.html
# with implementation by:
# Yuri Boykov (yuri@csd.uwo.ca)
# Vladimir Kolmogorov (vnk@ist.ac.at)
# 2001-2006
# TODO: switch to boost graph library

add_library(maxflow SHARED
    maxflow.cpp
)

target_include_directories(maxflow PUBLIC SYSTEM
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
)
set_target_properties(maxflow PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_STANDARD 20)
