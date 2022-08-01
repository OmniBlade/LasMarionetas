include(TargetExports)

add_library(demoplugin SHARED)
target_sources(demoplugin PRIVATE plugindirector.cpp)
target_link_libraries(demoplugin PRIVATE gzframework)
set_target_properties(demoplugin PROPERTIES OUTPUT_NAME TS2DemoPlugin PREFIX "")

target_exports(demoplugin SYMBOLS
    GZDllGetGZCOMDirector
)
