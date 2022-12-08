execute_process(
  COMMAND ${CPM} create test_project
  COMMAND_ERROR_IS_FATAL ANY
)
execute_process(
  COMMAND ${CPM} build
  COMMAND_ERROR_IS_FATAL ANY
  WORKING_DIRECTORY ./test_project
)
