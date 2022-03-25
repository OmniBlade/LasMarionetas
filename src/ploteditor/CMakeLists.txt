set(PLOTED_SRC
    main.cpp
)

if(DLLBUILD)
    include(TargetExports)
    add_library(parcela_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${PLOTED_SRC})
    target_link_libraries(parcela_lib PRIVATE gzframework_city)
    set_target_properties(parcela_lib PROPERTIES OUTPUT_NAME parcela PDB_NAME parceladll)

    target_exports(parcela_lib SYMBOLS
        SetupHooks
    )
endif()

if(STANDALONE)
    add_executable(parcela ${PLOTED_SRC})
    target_link_libraries(parcela PRIVATE gzframework)
endif()
