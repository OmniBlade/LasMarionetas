cmake_minimum_required(VERSION 3.19.0)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, supported options are: Debug Release" FORCE)
endif()

if(MSVC)
    set(MSVC_INCREMENTAL_DEFAULT ON)
endif()

project(lasmarionetas VERSION 1.00.0 LANGUAGES C CXX)

# We don't support in tree builds, so help people make the right choice.
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

include(FeatureSummary)

# This doesn't really work yet, work ongoing to make it usable
option(BUILD_PEOPLEGAME "Build the people simulator game." ON)
option(BUILD_CITYGAME "Build the city simulator game." OFF)
option(BUILD_PEOPLEEDITOR "Build the character editor utilitity." OFF)
option(BUILD_HOUSEEDITOR "Build the house editor utilitity." OFF)
option(BUILD_PLOTEDITOR "Build the city plot editor utilitity." OFF)
option(BUILD_PACKAGEINST "Build the package installer utilitity." OFF)
option(BUILD_PLUGIN "Build a demo extension plugin for the original game." OFF)
option(BUILD_TESTS "Build tests to check againt regressions." OFF)
option(BUILD_CLIUTILS "Build command line utils which may or maynot be useful." OFF)

add_feature_info(PeopleSim BUILD_PEOPLEGAME "People Simulation Game")
add_feature_info(CitySim BUILD_CITYGAME "City Simulation Game")
add_feature_info(PeopleEditor BUILD_PEOPLEEDITOR "Character Editor")
add_feature_info(HouseEditor BUILD_HOUSEEDITOR "Home/Building Editor")
add_feature_info(PlotEditor BUILD_PLOTEDITOR "City Plot Editor")
add_feature_info(PkgInstaller BUILD_PACKAGEINST "Packager Installer")
add_feature_info(DemoPlugin BUILD_PLUGIN "Demo/Example GZCOM Plugin Module")
add_feature_info(BuildTests BUILD_TESTS "Regression Tests")
add_feature_info(CliUtils BUILD_CLIUTILS "Command line utilities")

# Only MSVC generates the correct vtable layout for ABI compatibility needed for dll injection.
if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    option(DLLBUILD "Build an injectable dll version of selected programs." ON)
    option(STANDALONE "Build a standalone version of selected programs." OFF)
    add_feature_info(DllIjection DLLBUILD "ABI matching dlls for code injection/replacement")
    add_feature_info(Standalone STANDALONE "Standalone binaries")
else()
    set(STANDALONE TRUE)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${lasmarionetas_SOURCE_DIR}/cmake)

# Defines a global macro alias for getting the current function name to avoid ifdef magic later on.
include(CurrentFunction)

# Set up a format target to do automated clang format checking.
find_package(ClangFormat)
include(ClangFormat)

# Set where the build results will end up
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Find needed libraries
# Check for threading, win32 or posix.
find_package(Threads)

# Build lasmarionetas
add_subdirectory(src)

if(BUILD_TESTS)
    # Tests need to respect some options so need to occur after the options are set.
    enable_testing()
    add_subdirectory(tests)
endif()

feature_summary(WHAT ENABLED_FEATURES DESCRIPTION "Enabled features:")
feature_summary(WHAT DISABLED_FEATURES DESCRIPTION "Disabled features:")
