# Dependency libs needed to build the engine.
add_subdirectory(miniposix)
add_subdirectory(compat)
add_subdirectory(gimex)

# A fake d3d9.dll that injects a custom dll into the game related binaries.
if(DLLBUILD)
    add_subdirectory(hookproxy)
endif()

# The core app framework shared by all targets.
add_subdirectory(framework)

# A dll to use with the people simulator game to generate
# reference data for testing reimplementation correctness.
if(DLLBUILD)
    add_subdirectory(testgen)
endif()

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
if(PLUGIN)
    add_subdirectory(plugin)
endif()
