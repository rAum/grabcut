set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_GENERATOR MATCHES "Makefiles|Ninja" AND CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "No build type specified - set CMAKE_BUILD_TYPE=Release.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(
            CACHE CMAKE_BUILD_TYPE
            PROPERTY STRINGS Debug RelWithDebInfo Release MinSizeRel
    )
endif()

if (TRUE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
endif()

function(enable_project_settings Target)
    target_compile_options(${Target} PRIVATE "-Werror" "-Wall" "-Wpedantic" "-Wextra" "-Wno-unused-parameter")
endfunction()
