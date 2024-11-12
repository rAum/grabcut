pybind11_add_module(pygrabcut cpp/binding.cpp)
target_include_directories(pygrabcut PRIVATE cpp/include/)
target_link_libraries(pygrabcut PRIVATE grabcut)
target_compile_definitions(pygrabcut PRIVATE VERSION_INFO=${ver})
set_target_properties(
        pygrabcut
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
)
