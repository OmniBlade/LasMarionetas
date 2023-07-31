# Codex includes APIs as one of the libraries it provides.
find_package(ZLIB)

if(NOT ZLIB_FOUND AND NOT TARGET ZLIB::ZLIB)
    if(BINARY_MATCH) # Binary matching is a specific version of zlib which is shipped.
        add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/zlib ${CMAKE_BINARY_DIR}/thirdparty/zlib)
    else()
        include(FetchContent)
        FetchContent_Declare(
            zlib
            GIT_REPOSITORY https://github.com/madler/zlib.git
            GIT_TAG        v1.2.11
        )
    
        # We don't use FetchContent_MakeAvailable here because we don't want all zlib targets including, just our dependency.
        FetchContent_GetProperties(zlib)
        if(NOT zlib_POPULATED)
            FetchContent_Populate(zlib)
            add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()

        # Make sure headers are available for the static target and make an alias to match the Find module output.
        target_include_directories(zlibstatic INTERFACE ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
        add_library(ZLIB::ZLIB ALIAS zlibstatic)
        set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} PARENT_SCOPE)
        set(ZLIB_FOUND TRUE PARENT_SCOPE)
    endif()
endif()

add_library(codex STATIC)

target_sources(codex PRIVATE
    include/codex.h
    src/refpack.c
)

target_include_directories(codex PUBLIC src include)
target_link_libraries(codex PRIVATE compat)
target_link_libraries(codex PUBLIC ZLIB::ZLIB)
target_compile_definitions(codex PRIVATE -D_CRT_SECURE_NO_WARNINGS)
