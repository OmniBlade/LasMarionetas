add_subdirectory(gimex)
add_subdirectory(common)
add_subdirectory(editor)
add_subdirectory(game)
add_subdirectory(pkginstall)

if(PLUGIN)
    add_subdirectory(plugin)
endif()
