include(TargetExports)

add_library(proxycommon OBJECT hooker.cpp hooker.h mapview.cpp mapview.h sha1.cpp sha1.h)
target_link_libraries(proxycommon PUBLIC compat)

add_library(proxyd3d9 SHARED proxydll.cpp)
target_link_libraries(proxyd3d9 PRIVATE proxycommon)
set_target_properties(proxyd3d9 PROPERTIES OUTPUT_NAME d3d9 PDB_NAME proxyd3d9)
target_compile_definitions(proxyd3d9 PRIVATE FAKED3D $<$<CONFIG:DEBUG>:GAME_DEBUG>)
target_exports(proxyd3d9 SYMBOLS 
    Direct3DCreate9=FakeDirect3DCreate9
    Direct3DShaderValidatorCreate9=FakeDirect3DShaderValidatorCreate9
)

add_library(proxyvfw SHARED proxydll.cpp)
target_link_libraries(proxyvfw PRIVATE proxycommon)
set_target_properties(proxyvfw PROPERTIES OUTPUT_NAME msvfw32 PDB_NAME proxyvfw)
target_compile_definitions(proxyvfw PRIVATE FAKEVFW $<$<CONFIG:DEBUG>:GAME_DEBUG>)
target_exports(proxyvfw SYMBOLS 
    ICLocate=FakeICLocate
    ICDecompress=FakeICDecompress
    ICSendMessage=FakeICSendMessage
)

if(BUILD_CLIUTILS)
    add_executable(proxyhashgen hasher.cpp sha1.cpp sha1.h)
    target_link_libraries(proxyhashgen PRIVATE compat)
endif()
