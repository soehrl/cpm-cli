add_cpm_test("Create project" ${CMAKE_CURRENT_SOURCE_DIR}/create_project.cmake)
add_cpm_test("Create existing project" ${CMAKE_CURRENT_SOURCE_DIR}/create_project.cmake WILL_FAIL)
