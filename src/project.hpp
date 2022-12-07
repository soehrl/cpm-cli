#pragma once

#include <memory>
#include <optional>
#include <string>

#include "utils.hpp"
#include "target.hpp"

struct Project : std::enable_shared_from_this<Project> {
  using Ptr = std::shared_ptr<Project>;

  Path path;
  std::string name;

  static std::shared_ptr<Project> Open(const Path& path);
  static std::shared_ptr<Project> Create(const Path& parent_path, std::string_view project_name);

  std::shared_ptr<Target> AddTarget(std::string_view name, TargetType type);
  void AddDependency(std::string_view dependency_definition);
};
