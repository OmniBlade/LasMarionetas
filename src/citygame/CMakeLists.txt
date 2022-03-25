set(CITY_SRC
    main.cpp
)

if(DLLBUILD)
    include(TargetExports)
    add_library(ciudad_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${CITY_SRC})
    target_link_libraries(ciudad_lib PRIVATE gzframework_city)
    set_target_properties(ciudad_lib PROPERTIES OUTPUT_NAME ciudad PDB_NAME ciudaddll)
    
    target_exports(ciudad_lib SYMBOLS
        SetupHooks
    )
endif()

if(STANDALONE)
    add_executable(ciudad ${CITY_SRC})
    target_link_libraries(ciudad PRIVATE gzframework)
endif()
