# Dependency libs needed to build the engine.
add_subdirectory(compat)

#Set by compat
if(BINARY_MATCH)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/stlport ${CMAKE_BINARY_DIR}/thirdparty/stlport)
endif()

add_subdirectory(codex)
add_subdirectory(gimex)
add_subdirectory(allocator)

# The core app framework shared by all targets.
add_subdirectory(framework)

# The games/editors themselves.
if(BUILD_PEOPLEGAME)
    add_subdirectory(game)
endif()

if(BUILD_CITYGAME)
    add_subdirectory(citygame)
endif()

if(BUILD_PEOPLEEDITOR)
    add_subdirectory(charactereditor)
endif()

if(BUILD_HOUSEEDITOR)
    add_subdirectory(houseeditor)
endif()

if(BUILD_PLOTEDITOR)
    add_subdirectory(ploteditor)
endif()

if(BUILD_PACKAGEINST)
    add_subdirectory(pkginstall)
endif()

# A demo "plugin" dll that can extend a game without needing code injection.
# Can only be made to work for the actual games without code injection, not the editors.
if(BUILD_PLUGIN)
    add_subdirectory(plugin)
endif()
