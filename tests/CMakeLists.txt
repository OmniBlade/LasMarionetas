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

FetchContent_Declare(
    canterburycorpus
    GIT_REPOSITORY https://github.com/pfalcon/canterbury-corpus.git
    GIT_TAG        c380ef9dc1dc62691757fdfeebdc2f85710e1e46
)

FetchContent_GetProperties(canterburycorpus)
if(NOT zlib_POPULATED)
    FetchContent_Populate(canterburycorpus)
endif()

add_executable(test_lasmarionetas test_rzcmdline.cpp test_rzrandom.cpp test_codex.cpp)
target_link_libraries(test_lasmarionetas GTest::gtest GTest::gtest_main)
target_compile_definitions(test_lasmarionetas PRIVATE -DTESTDATA_PATH="${canterburycorpus_SOURCE_DIR}")

target_link_libraries(test_lasmarionetas gzframework codex)

include(GoogleTest)
gtest_discover_tests(test_lasmarionetas)
