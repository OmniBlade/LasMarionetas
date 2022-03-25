include(CheckSymbolExists)
check_symbol_exists(strtrim "string.h" HAVE_STRTRIM)
check_symbol_exists(strlwr "string.h" HAVE_STRLWR)
check_symbol_exists(strupr "string.h" HAVE_STRUPR)
check_symbol_exists(strlcat "string.h" HAVE_STRLCAT)
check_symbol_exists(strlcpy "string.h" HAVE_STRLCPY)
configure_file(string.h.in string.h @ONLY)

add_library(compat STATIC)

target_sources(compat PRIVATE
    aliasing.h
    endianness.h
    rotate.h
    string.c
    ${CMAKE_CURRENT_BINARY_DIR}/string.h
)

target_include_directories(compat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
