add_library(codex STATIC)

target_sources(codex PRIVATE
    include/codex.h
    src/refpack.c
)

target_include_directories(codex PUBLIC src include)
target_link_libraries(codex PRIVATE compat)
target_compile_definitions(codex PRIVATE -D_CRT_SECURE_NO_WARNINGS)
