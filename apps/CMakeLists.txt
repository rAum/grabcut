find_package(OpenCV 4 REQUIRED core imgproc highgui)

set(APP run_grabcut)

add_executable(${APP} run_grabcut.cpp)
target_include_directories(${APP} PRIVATE SYSTEM ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${APP} PRIVATE ${OpenCV_LIBS} grabcut)
enable_project_settings(${APP})
unset(APP)