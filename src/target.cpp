#include "target.hpp"

#include "spdlog/spdlog.h"
#include "utils.hpp"
#include "project.hpp"
#include "spdlog/fmt/bundled/format.h"

std::string GenerateExecutableSourceFile() {
  return 
R"(#include <iostream>

int main(int argc, char* argv[]) {
  std::cout << "Hello World!" << std::endl;
}
)";
}

std::string GenerateLibraryHeaderFile(std::string_view project_name) {
  using namespace fmt::literals;

  return fmt::format(
R"(#pragma once

namespace {project_name} {{
  int foo();
}}
)",
    "project_name"_a = project_name
  );
}

std::string GenerateLibrarySourceFile(std::string_view project_name, std::string_view target_name) {
  using namespace fmt::literals;

  return fmt::format(
R"(#include "{project_name}/{target_name}/{target_name}.hpp"

namespace {project_name} {{
  int foo() {{
    return 42;
  }}
}}
)",
    "project_name"_a = project_name,
    "target_name"_a = target_name
  );
}

std::string GenerateExecutableCMakeListsFile(std::string_view project_name, std::string_view target_name) {
  using namespace fmt::literals;

  return fmt::format(
R"(add_executable(
  {target_name}

  src/{target_name}.cpp
)
)",
    "project_name"_a = project_name,
    "target_name"_a = target_name
  );
}

std::string GenerateLibraryCMakeListsFile(std::string_view project_name, std::string_view target_name) {
  using namespace fmt::literals;

  return fmt::format(
R"(add_library(
  {project_name}-{target_name}

  include/{project_name}/{target_name}/{target_name}.hpp src/{target_name}.cpp
)

add_library({project_name}::{target_name} ALIAS {project_name}-{target_name})

target_include_directories(
  {project_name}-{target_name}
  PUBLIC
    include
)
)",
    "project_name"_a = project_name,
    "target_name"_a = target_name
  );
}


Target::Target(std::shared_ptr<Project> project, Path path, std::string name, TargetType type)
  : m_project(std::move(project)), m_path(std::move(path)), m_name(std::move(name)), m_type(type) {
}

std::shared_ptr<Target> Target::Create(std::shared_ptr<Project> project, std::string name, TargetType type) {
  const Path target_path = project->path / name;

  fs::create_directory(target_path);

  const auto source_path = target_path / "src";
  fs::create_directory(source_path);

  switch (type) {
    case TargetType::EXECUTABLE:
      WriteFile(target_path / "CMakeLists.txt", GenerateExecutableCMakeListsFile(project->name, name));
      WriteFile(source_path / fmt::format("{}.cpp", name), GenerateExecutableSourceFile());
      break;

    case TargetType::LIBRARY: {
      WriteFile(target_path / "CMakeLists.txt", GenerateLibraryCMakeListsFile(project->name, name));

      const auto include_path = target_path / "include" / project->name / name;
      fs::create_directories(include_path);
      WriteFile(include_path / fmt::format("{}.hpp", name), GenerateLibraryHeaderFile(project->name));

      WriteFile(source_path / fmt::format("{}.cpp", name), GenerateLibrarySourceFile(project->name, name));
      break;
    }

    case TargetType::NONE:
      spdlog::error("Invalid target type: none");
      break;
  }

  AppendFile(project->path / "CMakeLists.txt", fmt::format("\nadd_subdirectory({})", name));

  return std::shared_ptr<Target>(new Target(project, target_path, name, type));
}
