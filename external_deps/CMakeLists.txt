# This is a place to include all external libraries
# as code dependencies

if (BUILD_TESTS)
    message("Tests are enabled. Adding doctest")
    add_subdirectory(doctest)
endif()

add_subdirectory(stb)
add_subdirectory(corrade_lean)
add_subdirectory(eigen)