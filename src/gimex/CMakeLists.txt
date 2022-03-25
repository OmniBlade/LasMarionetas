# Get libjpeg or equivalent for JPEG handling.
find_package(JPEG)

if(NOT JPEG_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        turbojpeg_git
        GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
        GIT_TAG        2.1.0
    )

    # We don't use FetchContent_MakeAvailable here because we don't want all libjpeg targets including, just our dependencies.
    FetchContent_GetProperties(turbojpeg_git)
    if(NOT turbojpeg_git_POPULATED)
        FetchContent_Populate(turbojpeg_git)
        # We use the dynamic CRT runtime on windows, so make libjpeg match here.
        set(WITH_CRT_DLL TRUE)
        add_subdirectory(${turbojpeg_git_SOURCE_DIR} ${turbojpeg_git_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    add_library(libjpeg INTERFACE)
    target_link_libraries(libjpeg INTERFACE jpeg-static)
    target_include_directories(libjpeg INTERFACE ${turbojpeg_git_SOURCE_DIR} ${turbojpeg_git_BINARY_DIR})
    add_library(JPEG::JPEG ALIAS libjpeg)
endif()

# Get libpng for PNG handling.
find_package(PNG)

if(NOT PNG_FOUND)
    include(FetchContent)
    # Looks like we are building libpng ourselves, so we need zlib.
    find_package(ZLIB)

    if(NOT ZLIB_FOUND AND NOT TARGET ZLIB::ZLIB)
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
        set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
    endif()

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
