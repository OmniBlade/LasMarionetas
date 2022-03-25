set(CHARED_SRC
    main.cpp
)

file(GLOB_RECURSE CHARED_HEADERS "*.h")

if(DLLBUILD)
    include(TargetExports)
    add_library(tienda_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${CHARED_SRC})
    target_link_libraries(tienda_lib PRIVATE gzframework_people)
    set_target_properties(tienda_lib PROPERTIES OUTPUT_NAME tienda PDB_NAME tiendadll)

    target_exports(tienda_lib SYMBOLS
        SetupHooks
    )
endif()

if(STANDALONE)
    add_executable(tienda ${CHARED_SRC})
    target_link_libraries(tienda PRIVATE gzframework)
endif()
