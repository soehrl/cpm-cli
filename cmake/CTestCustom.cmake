set(CTEST_CUSTOM_PRE_TEST "${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/test_environment_template ${CMAKE_CURRENT_BINARY_DIR}/test_environment")
set(CTEST_CUSTOM_POST_TEST "${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_BINARY_DIR}/test_environment")
