add_custom_target(tests)
add_dependencies(tests test_rzcmdline)

add_executable(test_rzcmdline testcmdline.cpp)
target_link_libraries(test_rzcmdline PRIVATE common)
add_test(NAME test_rzcmdline COMMAND $<TARGET_FILE:test_rzcmdline>)
