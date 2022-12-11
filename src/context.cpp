#include "context.hpp"
#include "spdlog/spdlog.h"
#include "subprocess.hpp"
#include "toml++/toml.h"

#include <cstdlib>
#include <fstream>

Context g_context;

bool Context::Init() {
  g_context.paths.home = std::getenv("HOME");
  g_context.paths.config_directory = g_context.paths.home / ".local" / "share" / "cpm-cli";
  g_context.paths.config_file = g_context.paths.config_directory / "cpm-cli.toml";
  g_context.paths.cache = g_context.paths.home / ".cache" / "cpm-cli";
  g_context.paths.registries = g_context.paths.cache / "registries";
  g_context.paths.cpm_cache = g_context.paths.cache / "cpm_cache";

  if (!fs::exists(g_context.paths.config_directory) && !fs::create_directories(g_context.paths.config_directory)) {
    spdlog::error("Failed to create directory {}", g_context.paths.config_directory.string());
    return false;
  }

  if (!fs::exists(g_context.paths.registries) && !fs::create_directories(g_context.paths.registries)) {
    spdlog::error("Failed to create directory {}", g_context.paths.registries.string());
    return false;
  }

  if (!fs::exists(g_context.paths.config_file)) {
    spdlog::info("No user configuration file found. Create default configuration.");
    g_context.config.insert("registries", toml::table{
      {
        "cpm-cli",
        toml::table {
          { "repository", "https://github.com/soehrl/cpm-cli-registry.git" }
        }
      }
    });
    std::ofstream config_file(g_context.paths.config_file);
    config_file << g_context.config;
    if (!config_file.good()) {
      return false;
    }
  } else {
    g_context.config = toml::parse_file(g_context.paths.config_file.string());
  }

  return true;
}

bool Context::SetupRegistries() const {
  const auto registries = config.at("registries").as_table();
  if (!registries) {
    spdlog::warn("No package registries registered");
    return true;
  }

  for (const auto& [name, registry_config] : *registries) {
    const auto registry_path = g_context.paths.registries / name.str();
    const auto repository_uri = registry_config.as_table()->at("repository").value<std::string>();

    spdlog::info("Update registry {}", name.str());
    if (!fs::exists(registry_path)) {
      const auto clone = subprocess::Popen({ "git", "clone", repository_uri->c_str(), registry_path.c_str() }).wait();
    } else {
      const auto pull = subprocess::Popen({ "git", "pull" }, subprocess::cwd{ registry_path.c_str() }).wait();
    }
  }

  return true;
}
