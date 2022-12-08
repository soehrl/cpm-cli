macro(add_cpm_test name file)
  add_test(
    NAME ${name}
    COMMAND ${CMAKE_COMMAND} -DCPM=$<TARGET_FILE:cpm> -P ${file}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test_environment
  )

  if("${ARGN}" STREQUAL WILL_FAIL)
    set_tests_properties(
      ${name}
      PROPERTIES
        WILL_FAIL TRUE
    )
  endif()
endmacro()
