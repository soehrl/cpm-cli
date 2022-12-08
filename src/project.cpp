#include <optional>
#include <regex>

#include "project.hpp"
#include "cpm.cmake.hpp"
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;

std::string GenerateCMakeListsFile(std::string_view project_name) {
  return fmt::format(
R"(cmake_minimum_required(
  VERSION 3.14
)

project(
  {}
  VERSION 0.1.0
  LANGUAGES CXX
)

include(cmake/CPM.cmake)

)", project_name);
}

std::optional<std::string> ParseProjectName(std::string project_file_conent) {
  std::regex project_definition("project\\s*\\(\\s*(\\S+)");
  std::smatch match;
  std::regex_search(project_file_conent, match, project_definition);

  if (match.empty()) {
    return std::nullopt;
  } else {
    return match[1].str();
  }
}


std::shared_ptr<Project> Project::Open(const Path& path) {
  const auto cmakelists_file_path = path / "CMakeLists.txt";
  const auto cmakelists_file_content = ReadFile(cmakelists_file_path);

  if (!cmakelists_file_content.has_value()) {
    spdlog::error("The current folder does not seem to be a cmake project");
    return nullptr;
  }

  const auto project_name = ParseProjectName(*cmakelists_file_content);
  if (!project_name.has_value()) {
    spdlog::error("Cannot determine project name");
    return nullptr;
  }
  
  auto project = std::make_shared<Project>();
  project->name = *project_name;
  project->path = path;

  return project;
}

std::shared_ptr<Project> Project::Create(const Path& parent_path, std::string_view project_name) {
  auto project = std::make_shared<Project>();
  project->path = parent_path / project_name;

  if (!fs::create_directory(project->path)) {
    spdlog::error("Cannot create folder {}", project_name);
    return nullptr;
  }
  WriteFile(project->path / "CMakeLists.txt", GenerateCMakeListsFile(project_name));

  if (!fs::create_directory(project->path / "cmake")) {
    spdlog::error("Cannot create folder {}", "cmake");
  }
  WriteFile(project->path / "cmake" / "CPM.cmake", cpm_cmake);

  return project;
}

std::shared_ptr<Target> Project::AddTarget(std::string_view name, TargetType type) {
  return Target::Create(this->shared_from_this(), std::string(name), type);
}

void Project::AddDependency(std::string_view dependency) {
  spdlog::info("Adding a dependency");
}
