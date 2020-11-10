set(EDITOR_SRC
    main.cpp
)

file(GLOB_RECURSE EDITOR_HEADERS "*.h")

if(DLLBUILD)
    include(TargetExports)
    add_library(bodyshop_lib SHARED dllmain.cpp hookedfuncs.cpp hookedglobals.cpp ${EDITOR_SRC} ${EDITOR_HEADERS})
    target_link_libraries(bodyshop_lib PRIVATE commondll)
    set_target_properties(bodyshop_lib PROPERTIES OUTPUT_NAME oleaut32 PDB_NAME bodyshopdll)

    target_exports(bodyshop_lib SYMBOLS
        StartHooking
        StopHooking
        "VariantClear=FakeVariantClear @9"
        "VariantInit=FakeVariantInit @8"
    )
endif()

if(STANDALONE)
    add_executable(bodyshop ${EDITOR_SRC} ${EDITOR_HEADERS})
    target_link_libraries(bodyshop PRIVATE common)
endif()
