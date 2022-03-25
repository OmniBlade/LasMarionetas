if(TARGET gzframework_people)
    include(TargetExports)
    add_library(testgen_lib SHARED
        main.cpp
        dllmain.cpp
        hookedfuncs.cpp
        genrandom.cpp
        genrandom.h
    )
    target_link_libraries(testgen_lib PRIVATE gzframework_people)
    set_target_properties(testgen_lib PROPERTIES OUTPUT_NAME marionetas_testgen PDB_NAME testgendll)

    target_exports(testgen_lib SYMBOLS
        SetupHooks
    )
endif()
