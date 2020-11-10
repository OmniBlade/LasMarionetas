set(GAME_SRC
    main.cpp
)

file(GLOB_RECURSE GAME_HEADERS "*.h")

if(DLLBUILD)
    include(TargetExports)
    add_library(lasmarionetas_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${GAME_SRC} ${GAME_HEADERS})
    target_link_libraries(lasmarionetas_lib PRIVATE commondll)
    set_target_properties(lasmarionetas_lib PROPERTIES OUTPUT_NAME d3d9 PDB_NAME lasmarionetasdll)
    
    target_exports(lasmarionetas_lib SYMBOLS
        StartHooking
        StopHooking
        Direct3DCreate9=FakeDirect3DCreate9
        Direct3DShaderValidatorCreate9=FakeDirect3DShaderValidatorCreate9
    )
endif()

if(STANDALONE)
    add_executable(lasmarionetas ${GAME_SRC} ${GAME_HEADERS})
    target_link_libraries(lasmarionetas PRIVATE common)
endif()
