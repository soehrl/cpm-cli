cmake_minimum_required(
  VERSION 3.24
)

project(
  cpm-cli
  VERSION 0.1.0
  LANGUAGES CXX
)

include(CTest)
include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#9.1.0")
CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage("gh:CLIUtils/CLI11@2.3.1")
CPMAddPackage("gh:soehrl/subprocess#901ba4a")
CPMAddPackage("gh:nlohmann/json@3.10.5")
CPMAddPackage("gh:libcpr/cpr#1.9.3")
CPMAddPackage("gh:marzer/tomlplusplus@3.2.0")

add_executable(
  cpm

  src/cpm.cpp
  src/cmake.cpp
  src/cpm.cmake.cpp
  src/utils.cpp
  src/context.cpp
  src/registry.cpp

  src/project.cpp
  src/target.cpp

  src/commands/create.cpp
  src/commands/add.cpp
  src/commands/add_package.cpp
  src/commands/configure.cpp
  src/commands/build.cpp
)

target_link_libraries(
  cpm
  PRIVATE
    fmt::fmt
    spdlog::spdlog
    CLI11::CLI11
    subprocess::subprocess
    nlohmann_json::nlohmann_json
    cpr::cpr
    tomlplusplus::tomlplusplus
)

set_property(
  TARGET cpm 
  PROPERTY CXX_STANDARD 20
)

install(
  TARGETS cpm
)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  include(cmake/add_cpm_test.cmake)
  add_custom_command(
    TARGET cpm
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test_environment_template
  )
  add_custom_command(
    TARGET cpm
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf *
    COMMAND cpm create existing_project
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test_environment_template
  )
  configure_file(cmake/CTestCustom.cmake ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
  add_subdirectory(test)
endif ()
