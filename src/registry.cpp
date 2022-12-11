#include "registry.hpp"

#include <charconv>
#include <regex>
#include <fstream>

#include "context.hpp"
#include "cpr/api.h"
#include "cpr/cprtypes.h"
#include "nlohmann/json.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "cpr/cpr.h"
#include "spdlog/spdlog.h"




std::optional<RegisteredPackage> RegisteredPackage::Parse(const nlohmann::json& json) {
  RegisteredPackage package;
  if (json.contains("repository")) {
    const std::string& repository_string = json.at("repository");
    if (const auto repository = Repository::Parse(repository_string); repository) {
      package.repository = *repository;
    } else {
      return std::nullopt;
    }
  }
  if (json.contains("versionPrefix")) {
    package.version_prefix = json.at("versionPrefix");
  }
  return package;
}

std::optional<RegisteredPackage> FindPackage(std::string_view package_name) {
  const std::string package_filename = fmt::format("{}.json", package_name);

  g_context.SetupRegistries();

  for (const auto& [registry_name, _] : *g_context.config["registries"].as_table()) {
    const auto registry_path = g_context.paths.registries / registry_name.str();
    const auto registry_package_filename = registry_path / package_filename;
    if (fs::exists(registry_package_filename)) {
      std::ifstream package_file(registry_package_filename);
      return RegisteredPackage::Parse(nlohmann::json::parse(package_file));
    }
    fmt::print("{} not found in {}", package_name, registry_path.string());
  }

  return std::nullopt;
}
