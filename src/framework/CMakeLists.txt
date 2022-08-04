# This common library implements the base of the gonzo/razzo engine on which the other programs are built.
# Most project wide defines will be added at this level as all other targets should be linking against it.

# Embed some git version information in the binary.
set(GIT_PRE_CONFIGURE_FILE "gitinfo.c.in")
set(GIT_POST_CONFIGURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/gitinfo.c")
include(GitWatcher)

# Add a macro for the best function name printing compiler macro available.

set(FRAMEWORK_INCLUDES
    .
    gz
    rz
)

set(FRAMEWORK_SRC
    ${GIT_POST_CONFIGURE_FILE}
    gimexstd.cpp
    gitinfo.h
    gz/gzclsid.h
    gz/igzapp.h
    gz/igzbuffer.h
    gz/igzcmdline.h
    gz/igzcom.h
    gz/igzcomdirector.h
    gz/igzcomlibrary.h
    gz/igzcompressor.h
    gz/igzcursor.h
    gz/igzcursormanager.h
    gz/igzcursorresource.h
    gz/igzcursorresourcefactory.h
    gz/igzcriticalsection.h
    gz/igzexceptionnotification.h
    gz/igzfile.h
    gz/igzfilesystem.h
    gz/igzframework.h
    gz/igzframeworkhooks.h
    gz/igzframeworkw32.h
    gz/igzgraphicsystem.h
    gz/igzistream.h
    gz/igzkeyboard.h
    gz/igzmessageserver.h
    gz/igzmessagetarget.h
    gz/igzmessagetarget2.h
    gz/igzostream.h
    gz/igzpersistdbrecord.h
    gz/igzpersistdbsegment.h
    gz/igzpersistdbsegmentdirectoryfiles.h
    gz/igzpersistresource.h
    gz/igzpersistresourcekeyfilter.h
    gz/igzpersistresourcefactory.h
    gz/igzpersistdbserialrecord.h
    gz/igzserializable.h
    gz/igzstring.h
    gz/igzsystemservice.h
    gz/igzunknown.h
    gz/igzunknownenum.h
    gz/igzvariant.h
    rz/rzautorefcount.h
    rz/rzcmdline.cpp
    rz/rzcmdline.h
    rz/rzcomdlldirector.cpp
    rz/rzcomdlldirector.h
    rz/rzcriticalsection.h
    rz/rzdate.cpp
    rz/rzdate.h
    rz/rzfastalloc.cpp
    rz/rzfastalloc.h
    rz/rzrandom.cpp
    rz/rzrandom.h
    rz/rzrandomsuper.cpp
    rz/rzrandomsuper.h
    rz/rzrect.h
    rz/rzstring.cpp
    rz/rzstring.h
    rz/rzsysservptr.h
    rz/rzsystemservice.cpp
    rz/rzsystemservice.h
    rz/rzthreadsafe.cpp
    rz/rzthreadsafe.h
    rz/rztime.cpp
    rz/rztime.h
    rz/rzunknownmt.cpp
    rz/rzunknownmt.h
    unichar.h
)

set(FRAMEWORK_LIBS compat gimex)

if(WIN32 OR "${CMAKE_SYSTEM}" MATCHES "Windows")
    # Add thread releated implementations.
    if(Threads_FOUND)
        list(APPEND FRAMEWORK_SRC rz/rzcriticalsection_win32.cpp)
    endif()
else()
    # Add thread releated implementations.
    if(Threads_FOUND)
        list(APPEND FRAMEWORK_LIBS Threads::Threads)
        list(APPEND FRAMEWORK_SRC rz/rzcriticalsection_posix.cpp)
    endif()
endif()

file(GLOB_RECURSE FRAMEWORK_HEADERS "*.h")

if(STANDALONE OR PLUGIN OR BUILD_TESTS)
    add_library(gzframework STATIC ${FRAMEWORK_SRC})
    target_include_directories(gzframework PUBLIC ${FRAMEWORK_INCLUDES})
    add_dependencies(gzframework check_git)
    target_link_libraries(gzframework PUBLIC ${FRAMEWORK_LIBS})
    target_compile_features(gzframework PUBLIC cxx_std_17) # Pass the required standard down.
endif()

# DLL builds expose any globals as a reference that the injectable dll builds provide hooks against.
if(DLLBUILD)
    # Because of differences in the version of the game engine used, the ABI doesn't match for all targets.
    if(BUILD_PEOPLEGAME OR BUILD_PEOPLEEDITOR OR BUILD_PACKAGEINST)
        # Build to match ABI of people simulator and people editor.
        add_library(gzframework_people STATIC hooks/hooker.h hooks/gzhookedfuncs.cpp ${FRAMEWORK_SRC})
        # Original game uses STLPort instead of MSVC SDK for std namespace stuff. To match ABI we need to as well.
        # Note that this limits us to mostly C++98 level STL features.
        target_include_directories(gzframework_people PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stlport ${FRAMEWORK_INCLUDES} hooks)
        add_dependencies(gzframework_people check_git)
        target_compile_definitions(gzframework_people PUBLIC DLL_BUILD MATCH_ABI)
        target_link_libraries(gzframework_people PUBLIC ${FRAMEWORK_LIBS})
        target_compile_features(gzframework_people PUBLIC cxx_std_17) # Pass the required standard down.
    endif()

    if(BUILD_HOUSEEDITOR)
        # Build to match abi of house editor
        add_library(gzframework_house STATIC hooks/hooker.h hooks/gzhookedfuncs.cpp ${FRAMEWORK_SRC})
        # Original game uses STLPort instead of MSVC SDK for std namespace stuff. To match ABI we need to as well.
        # Note that this limits us to mostly C++98 level STL features.
        target_include_directories(gzframework_house PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stlport ${FRAMEWORK_INCLUDES} hooks)
        add_dependencies(gzframework_house check_git)
        target_compile_definitions(gzframework_house PUBLIC DLL_BUILD MATCH_ABI USE_HOUSE_ABI)
        target_link_libraries(gzframework_house PUBLIC ${FRAMEWORK_LIBS})
        target_compile_features(gzframework_house PUBLIC cxx_std_17) # Pass the required standard down.
    endif()

    if(BUILD_CITYGAME OR BUILD_MAPEDITOR)
        # Build to match abi of city simulator and plot editor
        add_library(gzframework_city STATIC hooks/hooker.h hooks/gzhookedfuncs.cpp ${FRAMEWORK_SRC})
        # Original game uses STLPort instead of MSVC SDK for std namespace stuff. To match ABI we need to as well.
        # Note that this limits us to mostly C++98 level STL features.
        target_include_directories(gzframework_city PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stlport ${FRAMEWORK_INCLUDES} hooks)
        add_dependencies(gzframework_city check_git)
        target_compile_definitions(gzframework_city PUBLIC DLL_BUILD MATCH_ABI USE_CITY_ABI)
        target_link_libraries(gzframework_city PUBLIC ${FRAMEWORK_LIBS})
        target_compile_features(gzframework_city PUBLIC cxx_std_17) # Pass the required standard down.
    endif()
endif()
