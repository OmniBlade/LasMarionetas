# Build file for character editor binary.
add_executable(tienda)
target_sources(tienda PRIVATE main.cpp)
target_link_libraries(tienda PRIVATE gzframework)
