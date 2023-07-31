add_library(allocator STATIC)

target_sources(allocator PRIVATE
    generalalloc.cpp
    include/allocator/allocmutex.h
    include/allocator/generalalloc.h
)
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(allocator PRIVATE allocmutex_win32.cpp)
else()
    target_sources(allocator PRIVATE allocmutex_posix.cpp)
endif()

target_include_directories(allocator PUBLIC include)
target_link_libraries(allocator PRIVATE compat)
