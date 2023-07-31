# Builds version found in TS2 for binary matching aspirations.
add_library(zlibstatic STATIC)
add_library(ZLIB::ZLIB ALIAS zlibstatic)

target_sources(zlibstatic PRIVATE
    adler32.c
    crc32.c
    deflate.c
    infblock.c
    infcodes.c
    inffast.c
    inflate.c
    inftrees.c
    infutil.c
    trees.c
    zutil.c
)

target_include_directories(zlibstatic PUBLIC .)
set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
set(ZLIB_FOUND TRUE PARENT_SCOPE)
