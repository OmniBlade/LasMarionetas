set(HOMEED_SRC
    main.cpp
)

if(DLLBUILD)
    include(TargetExports)
    add_library(constructor_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${HOMEED_SRC})
    target_link_libraries(constructor_lib PRIVATE gzframework_house)
    set_target_properties(constructor_lib PROPERTIES OUTPUT_NAME constructor PDB_NAME constructordll)

    target_exports(constructor_lib SYMBOLS
        SetupHooks
    )
endif()

if(STANDALONE)
    add_executable(constructor ${HOMEED_SRC})
    target_link_libraries(constructor PRIVATE gzframework)
endif()
