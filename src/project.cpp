#include <cstdlib>
#include <filesystem>
#include <optional>
#include <regex>

#include "CLI/Error.hpp"
#include "project.hpp"
#include "registry.hpp"
#include "spdlog/spdlog.h"
#include "subprocess.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

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

std::shared_ptr<Project> Project::Create(const Path& project_path, std::string_view template_definition) {
  auto project = std::make_shared<Project>();
  project->path = project_path;

  Repository repository;

  if (template_definition.length() > 0) {
    const auto parsed_repository = Repository::Parse(template_definition);
    if (!parsed_repository) {
      spdlog::error("Failed to parse repository url: {}", template_definition);
      return nullptr;
    } else {
      repository = std::move(*parsed_repository);
    }
  } else {
    repository.type = RepositoryType::GITHUB;
    repository.url = "https://github.com/TheLartians/ModernCppStarter.git";
    repository.name = "ModernCppStarter";
    repository.owner = "TheLartians";
  }

  const int clone_result = subprocess::Popen({ "git", "clone", "--recursive", "--no-tags", "--single-branch", repository.url.c_str(), project_path.c_str() }).wait();
  if (clone_result != 0) {
    spdlog::error("Failed to clone template");
    return nullptr;
  }

  if (const int result = subprocess::Popen({ "git", "remote", "remove", "origin"}, subprocess::cwd{project_path.c_str()}).wait(); result != 0) {
    spdlog::error("Failed to remove old remote origin");
    return nullptr;
  }

  const auto branch_name_buffer = subprocess::check_output({ "git", "branch", "--show-current"}, subprocess::cwd{project_path.c_str()});
  const std::string branch_name(branch_name_buffer.buf.data(), branch_name_buffer.length - 1);
  const std::string orphan_branch_name = fmt::format("{}-copy", branch_name);


  if (const int result = subprocess::Popen({ "git", "checkout", "--orphan", orphan_branch_name.c_str()}, subprocess::cwd{project_path.c_str()}).wait(); result != 0) {
    spdlog::error("Failed to create orphan branch");
    return nullptr;
  }

  if (const int result = subprocess::Popen({ "git", "branch", "-D", branch_name.c_str()}, subprocess::cwd{project_path.c_str()}).wait(); result != 0) {
    spdlog::error("Failed to delete old branch {}", branch_name);
    return nullptr;
  }

  if (const int result = subprocess::Popen({ "git", "branch", "-m", "main"}, subprocess::cwd{project_path.c_str()}).wait(); result != 0) {
    spdlog::error("Failed to move branch");
    return nullptr;
  }

  return project;
}

void Project::AddPackage(std::string_view package_definition) {
  const auto project_file_path = path / "CMakeLists.txt";
  auto project_file_content = ReadFile(project_file_path);
  if (!project_file_content ) {
    spdlog::error("Failed to read {}.", project_file_path.string());
    throw CLI::RuntimeError(-1);
  }

  std::string add_package_string;
  if (const auto repository = Repository::Parse(package_definition); repository) {
    add_package_string = repository->GetCPMDefinitionForLatestVersion();
  } else if (package_definition.find(':') == std::string::npos) {
    if (const auto package = FindPackage(package_definition); package) {
      add_package_string = package->repository.GetCPMDefinitionForLatestVersion(package->version_prefix);
    }
  } else {
    add_package_string = package_definition;
  }

  if (add_package_string.length() > 0) {
    project_file_content->insert(
      GetPackageInsertPosition(*project_file_content),
      fmt::format("\nCPMAddPackage(\"{}\")", add_package_string)
    );
    WriteFile(project_file_path, *project_file_content);
  }
}
