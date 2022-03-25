set(GAME_SRC
    main.cpp
)

if(DLLBUILD)
    include(TargetExports)
    add_library(marionetas_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${GAME_SRC})
    target_link_libraries(marionetas_lib PRIVATE gzframework_people)
    set_target_properties(marionetas_lib PROPERTIES OUTPUT_NAME marionetas PDB_NAME marionetasdll)
    
    target_exports(marionetas_lib SYMBOLS
        SetupHooks
    )
endif()

if(STANDALONE)
    add_executable(marionetas ${GAME_SRC})
    target_link_libraries(marionetas PRIVATE gzframework)
endif()
