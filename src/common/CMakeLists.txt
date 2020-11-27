# This common library implements the base of the gonzo/razzo engine on which the other programs are built.
# Most project wide defines will be added at this level as all other targets should be linking against it.

# Embed some git version information in the binary.
set(GIT_PRE_CONFIGURE_FILE "gitinfo.c.in")
set(GIT_POST_CONFIGURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/gitinfo.c")
include(GitWatcher)

set(COMMON_INCLUDES
    .
    gz
    rz
)

set(COMMON_SRC
    ${GIT_POST_CONFIGURE_FILE}
    rz/rzfastalloc.cpp
    rz/rzstring.cpp
)

set(COMMON_LIBS stringcompat)

if(WIN32 OR "${CMAKE_SYSTEM}" MATCHES "Windows")
    # Link our mini posix library on windows
    list(APPEND COMMON_LIBS miniposix)

    # Add thread releated implementations.
else()
    # Add thread releated implementations.
    if(Threads_FOUND)
        list(APPEND COMMON_LIBS Threads::Threads)
    endif()
endif()

file(GLOB_RECURSE COMMON_HEADERS "*.h")

if(STANDALONE)
    add_library(common STATIC ${COMMON_SRC} ${COMMON_HEADERS})
    target_include_directories(common PUBLIC ${COMMON_INCLUDES})
    add_dependencies(common check_git)

    # Link our mini posix library on windows unless we are using mingw which has implementations already.
    if(WIN32 OR "${CMAKE_SYSTEM}" MATCHES "Windows")
        if(NOT MINGW) 
            target_link_libraries(common PUBLIC miniposix)
        endif()
    endif()
endif()

# DLL builds expose any globals as a reference that the injectable dll builds provide hooks against.
if(DLLBUILD)
    add_library(commondll STATIC ${COMMON_SRC} ${COMMON_HEADERS})
    # Original game uses STLPort instead of MSVC SDK for std namespace stuff. To match ABI we need to as well.
    # Note that this limits us to mostly C++98 level STL features.
    target_include_directories(commondll PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/stlport ${COMMON_INCLUDES} hooks)
    add_dependencies(commondll check_git)
    target_compile_definitions(commondll PUBLIC DLL_BUILD)

    if(NOT MINGW) 
        target_link_libraries(commondll PUBLIC miniposix)
    endif()
endif()
