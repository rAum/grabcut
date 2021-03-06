# wrapper to simplify adding unit tests

if (BUILD_TESTS)
function (cpp_test NAME SOURCES)
    set(TEST_NAME "test_${NAME}")
    add_executable(${TEST_NAME} ${SOURCES})
    target_link_libraries(${TEST_NAME} PRIVATE doctest::doctest)
    add_test(${TEST_NAME} ${TEST_NAME})
    target_compile_definitions(${TEST_NAME} PRIVATE TESTDATA_DIR="${CMAKE_SOURCE_DIR}/tests/data" DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
    enable_project_settings(${TEST_NAME})
endfunction()
else()
    # if no testing, just stub the cmake function
    function (cpp_test NAME SOURCES LIBS)
    endfunction()
endif()