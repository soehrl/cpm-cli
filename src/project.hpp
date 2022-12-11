#pragma once

#include <memory>
#include <optional>
#include <string>

#include "utils.hpp"

struct Project : std::enable_shared_from_this<Project> {
  using Ptr = std::shared_ptr<Project>;

  Path path;
  std::string name;

  static std::shared_ptr<Project> Open(const Path& path);
  static std::shared_ptr<Project> Create(const Path& project_path, std::string_view template_definition = "");

  void AddPackage(std::string_view package_definition);
};
