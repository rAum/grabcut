
add_library(grabcut
        src/orchard-bouman.cpp
        src/grabcut.cpp
        src/segmentation_data.cpp
        src/fg_bg_graphcut.cpp
        src/learn_gmm.cpp
        )

target_link_libraries(grabcut PRIVATE
        Eigen3::Eigen
        maxflow
        )
target_compile_definitions(grabcut PUBLIC GRABCUT_VERSION="${PROJECT_VERSION}")
target_include_directories(grabcut PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
        )
target_include_directories(grabcut PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include/>)

set_property(TARGET grabcut PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET grabcut PROPERTY VISIBILITY_INLINES_HIDDEN ON)

add_library(grabcut::grabcut ALIAS grabcut)

option(INSTALL_GRABCUT_LIB "If set, install is enabled for grabcut library" OFF)
if (INSTALL_GRABCUT_LIB)
    install(TARGETS grabcut
            EXPORT grabcut-export
            RUNTIME
            DESTINATION bin
            COMPONENT grabcut
            LIBRARY
            DESTINATION lib
            COMPONENT grabcut
            ARCHIVE
            DESTINATION lib/static
            COMPONENT grabcut-dev
            PUBLIC_HEADER
            DESTINATION include
            COMPONENT grabcut-dev
            )

    install(DIRECTORY include/
            DESTINATION include/
            COMPONENT grabcut-dev
            )

    install(EXPORT ${PROJECT_NAME}-export
            FILE ${PROJECT_NAME}Config.cmake
            DESTINATION share/${PROJECT_NAME}
            COMPONENT board-core-dev)

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file("${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion)
    install(FILES
            "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION share/${PROJECT_NAME}
            COMPONENT grabcut-dev)
endif()
