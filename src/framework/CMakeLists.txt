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
    gz/gzcom.h
    gz/gzcom.cpp
    gz/gzcomlibrary.h
    gz/gzcomlibrary.cpp
    gz/gzframework.h
    gz/gzframework.cpp
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
    gz/igzpersistbufferresource.h
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
    rz/convertstring.cpp
    rz/convertstring.h
    rz/rzapppath.h
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
    rz/rzistream.cpp
    rz/rzistream.h
    rz/rzlocale.cpp
    rz/rzlocale.h
    rz/rzostream.cpp
    rz/rzostream.h
    rz/rzrandom.cpp
    rz/rzrandom.h
    rz/rzrandomsuper.cpp
    rz/rzrandomsuper.h
    rz/rzrect.h
    rz/rzsleep.h
    rz/rzstring.cpp
    rz/rzstring.h
    rz/rzsysservptr.h
    rz/rzsystemservice.cpp
    rz/rzsystemservice.h
    rz/rzthreadsafe.cpp
    rz/rzthreadsafe.h
    rz/rzthreadsafeobject.h
    rz/rztime.cpp
    rz/rztime.h
    rz/rztimer.cpp
    rz/rztimer.h
    rz/rzversion.cpp
    rz/rzversion.h
    rz/rzunknownmt.cpp
    rz/rzunknownmt.h
    unichar.h
)

add_library(gzframework STATIC ${FRAMEWORK_SRC})
target_include_directories(gzframework PUBLIC ${FRAMEWORK_INCLUDES})
add_dependencies(gzframework check_git)
target_link_libraries(gzframework PUBLIC compat gimex)

find_package(ICU COMPONENTS data i18n io tu uc)

if(NOT WINDOWS AND NOT ICU_FOUND)
    message(FATAL_ERROR "ICU is required on non-windows platforms and was not found.")
endif()

if(ICU_FOUND)
    target_link_libraries(gzframework PUBLIC ICU::data ICU::i18n ICU::io ICU::tu ICU::uc)
    target_compile_definitions(gzframework PUBLIC BUILD_WITH_ICU)
endif()

if(WINDOWS)
    # Add thread releated implementations.
    if(Threads_FOUND)
        target_sources(gzframework PRIVATE rz/rzcriticalsection_win32.cpp)
    endif()

    target_sources(gzframework PRIVATE rz/rzapppath_win32.cpp)
    target_link_libraries(gzframework PUBLIC powrprof version)
    target_compile_definitions(gzframework PUBLIC NOMINMAX)
else()
    # Add thread releated implementations.
    if(Threads_FOUND)
        target_sources(gzframework PRIVATE rz/rzcriticalsection_posix.cpp)
        target_link_libraries(gzframework PUBLIC Threads::Threads)
    endif()
    
    target_sources(gzframework PRIVATE rz/rzapppath_posix.cpp)
endif()

# When trying to binary match, we need to make sure the correct game ABI is generated.
if(BINARY_MATCH)
    target_link_libraries(gzframework PUBLIC stlport)
    # Because of differences in the version of the game engine used, the ABI doesn't match for all targets.
    if(BUILD_PEOPLEGAME OR BUILD_PEOPLEEDITOR OR BUILD_PACKAGEINST) # TS2 or its editor and mod installer.
        target_compile_definitions(gzframework PUBLIC MATCH_ABITS)
    elseif(BUILD_HOUSEEDITOR) # Homecrafter, probably lowest priority.
        target_compile_definitions(gzframework PUBLIC MATCH_ABIHC)
    else() # SC4 or its editor.
        target_compile_definitions(gzframework PUBLIC MATCH_ABISC)
    endif()
endif()
