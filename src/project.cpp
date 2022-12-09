#include <cstdlib>
#include <optional>
#include <regex>

#include "CLI/Error.hpp"
#include "project.hpp"
#include "cpm.cmake.hpp"
#include "registry.hpp"
#include "spdlog/spdlog.h"
#include "utils.hpp"

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

void ParsePackages(std::string_view project_file_content) {
  std::smatch match;
  std::string remaining_content(project_file_content);
  std::regex package_definition(R"(CPMAddPackage\s*\(\s*\"(\S+)\"\s*\))");
  while (std::regex_search(remaining_content, match, package_definition)) {
    spdlog::info("Found package {}", match[1].str());
    remaining_content = match.suffix();
  }
}

size_t GetPackageInsertPosition(std::string_view project_file_content) {
  std::smatch match;
  std::string remaining_content(project_file_content);

  std::regex include_cpm_definition(R"(include\s*\(\s*.*CPM\.cmake\s*\))");
  if (std::regex_search(remaining_content, match, include_cpm_definition)) {
    remaining_content = match.suffix();
  } else {
    spdlog::error("Project does not seem to use CPM.");
    throw CLI::RuntimeError(-1);
  }

  std::regex package_definition(R"(CPMAddPackage\s*\(\s*\"(\S+)\"\s*\))");
  while (std::regex_search(remaining_content, match, package_definition)) {
    remaining_content = match.suffix();
  }

  return project_file_content.size() - remaining_content.size();
}

std::shared_ptr<Project> Project::Open(const Path& path) {
  const auto cmakelists_file_path = path / "CMakeLists.txt";

  std::shared_ptr<Project> project;
  if (!fs::exists(cmakelists_file_path)) {
    if (!path.has_parent_path()) {
      project = Open(path.parent_path());
    }
  } else {
    const auto cmakelists_file_content = ReadFile(cmakelists_file_path);

    if (!cmakelists_file_content.has_value()) {
      spdlog::error("Failed to open {}", cmakelists_file_path.string());
    } else {
      const auto project_name = ParseProjectName(*cmakelists_file_content);
      if (!project_name.has_value()) {
        if (path.has_parent_path()) {
          project = Open(path.parent_path());
        }
      } else {
        project = std::make_shared<Project>();
        project->name = *project_name;
        project->path = path;
      }
    }
  }

  if (!project) {
    spdlog::error("The current folder does not seem to be a cmake project");
  }

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

void Project::AddPackage(std::string_view package_definition) {
  const auto project_file_path = path / "CMakeLists.txt";
  auto project_file_content = ReadFile(project_file_path);
  if (!project_file_content ) {
    spdlog::error("Failed to read {}.", project_file_path.string());
    throw CLI::RuntimeError(-1);
  }

  std::string add_package_string(package_definition);
  if (const auto repository = Repository::Parse(package_definition); repository) {
    add_package_string = repository->GetCPMDefinitionForLatestVersion();
  } else if (package_definition.find(':') == std::string::npos) {
    if (const auto package = FindPackage(package_definition); package) {
      add_package_string = package->repository.GetCPMDefinitionForLatestVersion();
    }
  }

  project_file_content->insert(
    GetPackageInsertPosition(*project_file_content),
    fmt::format("\nCPMAddPackage(\"{}\")", add_package_string)
  );
  WriteFile(project_file_path, *project_file_content);
}
