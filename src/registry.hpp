#pragma once

#include <cstdint>
#include "nlohmann/json.hpp"
#include "repository.hpp"

struct RegisteredPackage {
  Repository repository;
  std::string version_prefix;

  static std::optional<RegisteredPackage> Parse(const nlohmann::json& json);

};

std::optional<RegisteredPackage> FindPackage(std::string_view package_name);
