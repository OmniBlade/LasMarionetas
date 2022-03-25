find_package(GTest CONFIG)

if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.11.0
    )

    FetchContent_MakeAvailable(googletest)
    # We don't use FetchContent_MakeAvailable here because we don't want all libpng targets including, just our dependency.
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
        # Add some aliases to match the output of the find module.
        add_library(GTest::gtest ALIAS gtest)
        add_library(GTest::gtest_main ALIAS gtest_main)
    endif()
endif()

add_executable(test_lasmarionetas test_rzcmdline.cpp test_rzrandom.cpp)
target_link_libraries(test_lasmarionetas GTest::gtest GTest::gtest_main)
target_compile_definitions(test_lasmarionetas PRIVATE -DTESTDATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data")

target_link_libraries(test_lasmarionetas gzframework)

include(GoogleTest)
gtest_discover_tests(test_lasmarionetas)
