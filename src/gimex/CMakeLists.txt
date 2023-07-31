# Get libjpeg or equivalent for JPEG handling.
find_package(JPEG)

if(NOT JPEG_FOUND)
    if(BINARY_MATCH) # Binary matching is a specific version of libjpeg which is shipped.
        add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/jpeg ${CMAKE_BINARY_DIR}/thirdparty/jpeg)
    else()
        include(ExternalProject)
        ExternalProject_Add(libjpeg-turbo
                      CMAKE_ARGS
                        -DENABLE_SHARED=OFF
                        -DENABLE_STATIC=ON
                        -DWITH_TURBOJPEG=OFF
                        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libjpeg-turbo
                        -DCMAKE_INSTALL_LIBDIR=${CMAKE_BINARY_DIR}/bin
                        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                        GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
                        GIT_TAG        3.0.0)

        add_library(libjpeg INTERFACE)

        if(WIN32) 
            target_link_libraries(libjpeg INTERFACE ${CMAKE_BINARY_DIR}/bin/jpeg-static.lib)
        else()
            target_link_libraries(libjpeg INTERFACE ${CMAKE_BINARY_DIR}/bin/libjpeg.a)
        endif()
        target_include_directories(libjpeg INTERFACE ${libjpeg-turbo_SOURCE_DIR} ${libjpeg-turbo_BINARY_DIR})
        add_library(JPEG::JPEG ALIAS libjpeg)
    endif()
endif()

# Get libpng for PNG handling.
find_package(PNG)

if(NOT PNG_FOUND)
    if(BINARY_MATCH) # Binary matching is a specific version of libjpeg which is shipped.
        add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/png ${CMAKE_BINARY_DIR}/thirdparty/png)
    else()
        include(FetchContent)
        set(PNG_BUILD_ZLIB ON CACHE BOOL "Custom zlib Location, else find_package is used")
        set(SKIP_INSTALL_ALL ON)
        set(ZLIB_LIBRARY ZLIB::ZLIB)

        FetchContent_Declare(
            libpng
            GIT_REPOSITORY  https://github.com/glennrp/libpng.git
            GIT_TAG        v1.6.37
        )

        # We don't use FetchContent_MakeAvailable here because we don't want all libpng targets including, just our dependency.
        FetchContent_GetProperties(libpng)
        if(NOT libpng_POPULATED)
            FetchContent_Populate(libpng)
            add_subdirectory(${libpng_SOURCE_DIR} ${libpng_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()

        # Make sure headers are available for the static target and make an alias to match the Find module output.
        add_library(libpng INTERFACE)
        target_link_libraries(libpng INTERFACE png_static)
        target_include_directories(libpng INTERFACE ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})
        add_library(PNG::PNG ALIAS libpng)
    endif()
endif()

add_library(gimex STATIC)

target_sources(gimex PRIVATE
    include/gimex.h
    src/bitmap.h
    src/bitmapgimex.c
    src/bitmapgimex.h
    src/gfuncs.c
    src/gfuncs.h
    src/gimex.c
    src/jpeggimex.c
    src/jpeggimex.h
    src/jpegio.c
    src/jpegio.h
    src/nullgimex.c
    src/nullgimex.h
    src/pnggimex.c
    src/pnggimex.h
    src/shapefile.c
    src/shapefile.h
    src/shpgimex.c
    src/shpgimex.h
    src/targa.h
    src/targagimex.c
    src/targagimex.h
)

target_include_directories(gimex PUBLIC src include)
target_link_libraries(gimex PRIVATE compat JPEG::JPEG PNG::PNG)
target_compile_definitions(gimex PRIVATE -D_CRT_SECURE_NO_WARNINGS)

if(BUILD_CLIUTILS)
    add_executable(gimexconv src/gimexconv.c)
    target_link_libraries(gimexconv PRIVATE gimex compat)
    target_compile_definitions(gimexconv PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
