set(PKG_SRC
    main.cpp
)

if(DLLBUILD)
    include(TargetExports)
    add_library(pkginstaller_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${PKG_SRC})
    target_link_libraries(pkginstaller_lib PRIVATE gzframework_people)
    set_target_properties(pkginstaller_lib PROPERTIES OUTPUT_NAME pkginstaller PDB_NAME pkginstallerdll)
    
    target_exports(pkginstaller_lib SYMBOLS
        SetupHooks
    )
endif()

if(STANDALONE)
    add_executable(pkginstaller ${PKG_SRC})
    target_link_libraries(pkginstaller PRIVATE gzframework)
endif()
