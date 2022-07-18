# Read a file to read version
file(READ ${CMAKE_SOURCE_DIR}/version.txt ver)
string(STRIP ${ver} ver)
message(STATUS "Project version: ${ver}")