if (USE_SYSTEM_EIGEN)
    find_package(Eigen3 REQUIRED NO_MODULE)
    # below is a workaround for some buggy Eigen exports which managed to get into some distros
    if(NOT TARGET Eigen3::Eigen)
        message("Eigen has buggy exported package, but headers are found: ${EIGEN3_INCLUDE_DIRS}. Applying workaround.")
        add_library(Eigen INTERFACE IMPORTED GLOBAL)
        target_include_directories(Eigen SYSTEM INTERFACE "${EIGEN3_INCLUDE_DIRS}")
        add_library(Eigen3::Eigen ALIAS Eigen)
    else()
        get_target_property(EIGEN_INCLUDES_VERIFY Eigen3::Eigen INCLUDE_DIRECTORIES)
        if (NOT EIGEN_INCLUDES_VERIFY)
            target_include_directories(Eigen3::Eigen SYSTEM INTERFACE "${EIGEN3_INCLUDE_DIRS}")
            message(WARNING "Seems something is broken with Eigen, so applying workaround...")
        endif()
    endif()
else()
    add_library(Eigen3 IMPORTED INTERFACE GLOBAL)
    target_include_directories(Eigen3 SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/eigen-3.4.0")
    add_library(Eigen3::Eigen ALIAS Eigen3)
endif()