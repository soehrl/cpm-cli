cmake_minimum_required(
  VERSION 3.24
)

project(
  cpm-cli
  VERSION 0.1.0
  LANGUAGES CXX
)

include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#9.1.0")
CPMAddPackage("gh:gabime/spdlog@1.11.0")
CPMAddPackage("gh:CLIUtils/CLI11@2.3.1")
CPMAddPackage("gh:arun11299/cpp-subprocess#cbc9e82")

add_executable(
  cpm

  src/cpm.cpp
  src/cmake.cpp
  src/cpm.cmake.cpp
  src/utils.cpp

  src/project.cpp
  src/target.cpp

  src/commands/create.cpp
  src/commands/add.cpp
  src/commands/configure.cpp
  src/commands/build.cpp
)

target_link_libraries(
  cpm
  PRIVATE
    fmt::fmt
    spdlog::spdlog
    CLI11::CLI11
    subprocess
)

set_property(
  TARGET cpm 
  PROPERTY CXX_STANDARD 20
)
