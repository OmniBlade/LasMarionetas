# Build file for people simulator game binary.
add_executable(marionetas)
target_sources(marionetas PRIVATE main.cpp)
target_link_libraries(marionetas PRIVATE gzframework)
