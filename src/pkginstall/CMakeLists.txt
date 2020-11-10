set(PKG_SRC
    main.cpp
)

file(GLOB_RECURSE PKG_HEADERS "*.h")

if(DLLBUILD)
    include(TargetExports)
    add_library(pkginstaller_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${PKG_SRC} ${PKG_HEADERS})
    target_link_libraries(pkginstaller_lib PRIVATE commondll)
    set_target_properties(pkginstaller_lib PROPERTIES OUTPUT_NAME d3d9_pi PDB_NAME pkginstallerdll)
    
    target_exports(pkginstaller_lib SYMBOLS
        StartHooking
        StopHooking
        Direct3DCreate9=FakeDirect3DCreate9
        Direct3DShaderValidatorCreate9=FakeDirect3DShaderValidatorCreate9
    )
endif()

if(STANDALONE)
    add_executable(pkginstaller ${PKG_SRC} ${PKG_HEADERS})
    target_link_libraries(pkginstaller PRIVATE common)
endif()
