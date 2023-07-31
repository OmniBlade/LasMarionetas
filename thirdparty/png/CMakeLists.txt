# Builds version found in TS2 for binary matching aspirations.
add_library(png STATIC)
add_library(PNG::PNG ALIAS png)

target_sources(png PRIVATE
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngrio.c
    pngread.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

target_include_directories(png PUBLIC .)
target_link_libraries(png PUBLIC ZLIB::ZLIB)
