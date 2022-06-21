add_library(stb_utils STATIC implementation.cpp)
target_include_directories(stb_utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
set_target_properties(stb_utils PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        CXX_STANDARD 20)