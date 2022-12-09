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


std::optional<SemanticVersion> SemanticVersion::Parse(const std::string& version_string) {
  std::regex semantic_version_regex(R"(^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$)");
  std::smatch match;
  if (std::regex_match(version_string, match, semantic_version_regex)) {
    return SemanticVersion {
      .major = std::stoul(match[1].str()),
      .minor = std::stoul(match[2].str()),
      .patch = std::stoul(match[3].str()),
      .pre_release = match[4].str(),
      .build_metadata = match[4].str(),
    };
  } else {
    return std::nullopt;
  }
}

bool operator<(const SemanticVersion& lhs, const SemanticVersion& rhs) {
  if (lhs.major != rhs.major) {
    return lhs.major < rhs.major;
  } else if (lhs.minor != rhs.minor) {
    return lhs.minor < rhs.minor;
  } else if (lhs.patch != rhs.patch) {
    return lhs.patch < rhs.patch;
  } else if (lhs.pre_release != rhs.pre_release) {
    return lhs.pre_release < rhs.pre_release;
  } else {
    return false;
  }
}

std::string TaggedVersion::GetCPMSuffix() const {
  if (tag.starts_with('v')) {
    return fmt::format("@{}", tag.substr(1));
  } else {
    return fmt::format("#{}", tag);
  }
}

std::optional<Repository> Repository::Parse(std::string_view uri) {
  Repository repository { .uri = std::string(uri) };

  std::regex github_regex(R"(^https://github.com/(\S+)/(\S+)((/.*)|(\.git))?$)");

  std::smatch match;
  if (std::regex_match(repository.uri, match, github_regex)) {
    repository.type = RepositoryType::GITHUB;
    repository.owner = match[1].str();
    repository.name = match[2].str();
  }

  return repository;
}

std::vector<TaggedVersion> Repository::QueryVersions() const {
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
            if (const auto version = SemanticVersion::Parse(tag_name.starts_with('v') ? tag_name.substr(1) : tag_name); version) {
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

std::optional<TaggedVersion> Repository::QueryLatestVersion() const {
  if (const auto versions = QueryVersions(); versions.size() > 0) {
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

std::string Repository::GetCPMDefinitionForLatestVersion() const {
  return GetCPMDefinition(QueryLatestVersion());
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
  return package;
}

std::optional<RegisteredPackage> FindPackage(std::string_view package_name) {
  const std::string package_filename = fmt::format("{}.json", package_name);

  for (const auto& registry_path : g_context.paths.registries) {
    const auto registry_package_filename = registry_path / package_filename;
    if (fs::exists(registry_package_filename)) {
      std::ifstream package_file(registry_package_filename);
      return RegisteredPackage::Parse(nlohmann::json::parse(package_file));
    }
    fmt::print("{} not found in {}", package_name, registry_path.string());
  }

  return std::nullopt;
}
