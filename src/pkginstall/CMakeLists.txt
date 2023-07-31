# Build file for package installer binary.
add_executable(pkginstaller)
target_sources(pkginstaller PRIVATE main.cpp)
target_link_libraries(pkginstaller PRIVATE gzframework)
