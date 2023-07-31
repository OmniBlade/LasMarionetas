# Build file for terrain editor binary.
add_executable(parcela)
target_sources(parcela PRIVATE main.cpp)
target_link_libraries(parcela PRIVATE gzframework)
