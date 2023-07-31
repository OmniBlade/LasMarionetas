# Build file for house building editor binary.
add_executable(constructor)
target_sources(constructor PRIVATE main.cpp)
target_link_libraries(constructor PRIVATE gzframework)
