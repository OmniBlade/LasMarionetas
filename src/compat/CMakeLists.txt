include(CheckIncludeFile)

# Check if we are missing any posix headers we consider required.
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(libgen.h HAVE_LIBGEN_H)
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(fnmatch.h HAVE_FNMATCH_H)
check_include_file(getopt.h HAVE_GETOPT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(dlfcn.h HAVE_DLFCN_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)

include(CheckSymbolExists)

# Check if we are missing any functions known to exist in platform specific runtimes.
check_symbol_exists(strtrim "string.h" HAVE_STRTRIM)
check_symbol_exists(strlwr "string.h" HAVE_STRLWR)
check_symbol_exists(strupr "string.h" HAVE_STRUPR)
check_symbol_exists(strlcat "string.h" HAVE_STRLCAT)
check_symbol_exists(strlcpy "string.h" HAVE_STRLCPY)
configure_file(string.h.in string.h @ONLY)

# Check for posix high res timer.
check_symbol_exists(clock_gettime "time.h" HAVE_CLOCKGETTIME)
configure_file(time.h.in time.h @ONLY)

add_library(compat STATIC)
target_compile_options(compat PUBLIC ${MARIONETAS_CXX_FLAGS})

if(WIN32)
    target_compile_definitions(compat PUBLIC _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN)

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "13.10.3077")
            set(BINARY_MATCH TRUE PARENT_SCOPE)
            target_compile_definitions(compat PUBLIC BINARY_MATCH)
        elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "16.0.0")
            message(FATAL_ERROR "Las Marionetas does not support this version of MSVC.")
        else()
            target_compile_features(compat PUBLIC cxx_std_17 c_std_11) # Pass the required standard down.
        endif()
    endif()
endif()

target_sources(compat PRIVATE
    compat/aliasing.h
    compat/callconv.h
    compat/cpuid.h
    compat/endianness.h
    compat/rotate.h
    string.c
	time.c
    ${CMAKE_CURRENT_BINARY_DIR}/string.h
    ${CMAKE_CURRENT_BINARY_DIR}/time.h
)

target_include_directories(compat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/compat ${CMAKE_CURRENT_BINARY_DIR})

if(NOT HAVE_STRINGS_H)
    target_sources(compat PRIVATE strings/strings.c strings/strings.h)
    target_include_directories(compat PUBLIC strings)
endif()

if(NOT HAVE_LIBGEN_H)
    target_sources(compat PRIVATE libgen/libgen.c libgen/libgen.h)
    target_include_directories(compat PUBLIC libgen)
endif()

if(NOT HAVE_DIRENT_H)
    target_sources(compat PRIVATE dirent/dirent.c dirent/dirent.h)
    target_include_directories(compat PUBLIC dirent)
endif()

if(NOT HAVE_FNMATCH_H)
    target_sources(compat PRIVATE fnmatch/fnmatch.c fnmatch/fnmatch.h)
    target_include_directories(compat PUBLIC fnmatch)
endif()

if(NOT HAVE_GETOPT_H)
    target_sources(compat PRIVATE getopt/getopt.c getopt/getopt.h)
    target_include_directories(compat PUBLIC getopt)
endif()

if(NOT HAVE_UNISTD_H)
    target_sources(compat PRIVATE unistd/unistd.c unistd/unistd.h)
    target_include_directories(compat PUBLIC unistd)
endif()

if(NOT HAVE_DLFCN_H)
    target_sources(compat PRIVATE dlfcn/dlfcn.c dlfcn/dlfcn.h)
    target_include_directories(compat PUBLIC dlfcn)
endif()

if(NOT HAVE_SYS_TIME_H)
    target_sources(compat PRIVATE systime/time.c systime/sys/time.h)
    target_include_directories(compat PUBLIC systime)
endif()

if(NOT HAVE_STDINT_H)
    target_sources(compat PRIVATE stdint/stdint.h stdint/inttypes.h)
    target_include_directories(compat PUBLIC stdint)
endif()

if(NOT HAVE_STDBOOL_H)
    target_sources(compat PRIVATE stdbool/stdbool.h)
    target_include_directories(compat PUBLIC stdbool)
endif()
