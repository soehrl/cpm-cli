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



std::optional<Repository> Repository::Parse(std::string_view uri) {
  Repository repository { .url = std::string(uri) };

  std::regex github_regex(R"(^https://github.com/(\S+)/(\S+)((/.*)|(\.git))?$)");

  std::smatch match;
  if (std::regex_match(repository.url, match, github_regex)) {
    repository.type = RepositoryType::GITHUB;
    repository.owner = match[1].str();
    repository.name = match[2].str();
    return repository;
  } else {
    return std::nullopt;
  }
}

std::vector<TaggedVersion> Repository::QueryVersions(std::string_view version_prefix) const {
  std::vector<TaggedVersion> versions;

  switch (type) {
    case RepositoryType::OTHER:
      break;

    case RepositoryType::GITHUB: {
        const auto result = cpr::Get(
          cpr::Url{ fmt::format("https://api.github.com/repos/{}/{}/tags", owner, name) }
        );
        if (result.status_code == 200) {
          const auto tags = nlohmann::json::parse(result.text);
          for (const auto& tag : tags) {
            const std::string& tag_name = tag["name"];

            std::string version_string;
            if (version_prefix.length() > 0) {
              if (tag_name.starts_with(version_prefix)) {
                version_string = tag_name.substr(version_prefix.length());
              }
            } else {
              // The v prefix is extremely common (cpm event assumes it by default) so test for it.
              version_string = tag_name.starts_with('v') ? tag_name.substr(1) : tag_name;
            }

            if (const auto version = SemanticVersion::Parse(version_string); version) {
              versions.push_back({
                .tag = tag_name,
                .version = *version,
              });
            }
          }
        } else {
          spdlog::error("Failed to query tags ({}): {}", result.status_code, result.text);
        }
        break;
      }

    case RepositoryType::GITLAB:
      break;

    case RepositoryType::BITBUCKET:
      break;
  }

  std::sort(versions.begin(), versions.end(), [](const auto& lhs, const auto& rhs) { return lhs.version < rhs.version; });

  return versions;
}

std::optional<TaggedVersion> Repository::QueryLatestVersion(std::string_view version_prefix) const {
  if (const auto versions = QueryVersions(version_prefix); versions.size() > 0) {
    return versions.back();
  } else {
    return std::nullopt;
  }
}

std::string Repository::GetCPMDefinition(const std::optional<TaggedVersion>& version) const {
  const std::string version_suffix = version ? version->GetCPMSuffix() : "";
  switch (type) {
    case RepositoryType::OTHER:
      throw std::runtime_error("Not implemented yet");

    case RepositoryType::GITHUB:
      return fmt::format("gh:{}/{}{}", owner, name, version_suffix);

    case RepositoryType::GITLAB:
      throw std::runtime_error("Not implemented yet");

    case RepositoryType::BITBUCKET:
      throw std::runtime_error("Not implemented yet");
  }
}

std::string Repository::GetCPMDefinitionForLatestVersion(std::string_view version_prefix) const {
  return GetCPMDefinition(QueryLatestVersion(version_prefix));
}

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
