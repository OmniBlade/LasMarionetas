# Build file for city building game binary.
add_executable(ciudad)
target_sources(ciudad PRIVATE main.cpp)
target_link_libraries(ciudad PRIVATE gzframework)
