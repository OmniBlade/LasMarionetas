include(TargetExports)
include(CurrentFunction)

add_library(elplugin SHARED)
target_sources(elplugin PRIVATE plugindirector.cpp)
target_link_libraries(elplugin PRIVATE common)
set_target_properties(elplugin PROPERTIES OUTPUT_NAME TS2Plugin PREFIX "")

target_exports(elplugin SYMBOLS
    GZDllGetGZCOMDirector
)

target_current_function_define(elplugin)
