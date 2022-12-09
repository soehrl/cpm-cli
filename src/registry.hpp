#pragma once

#include <cstdint>
#include "nlohmann/json.hpp"

struct SemanticVersion {
  unsigned long major;
  unsigned long minor;
  unsigned long patch;
  std::string pre_release;
  std::string build_metadata;

  static std::optional<SemanticVersion> Parse(const std::string& version_string);
};

bool operator<(const SemanticVersion& lhs, const SemanticVersion& rhs);

struct TaggedVersion {
  std::string tag;
  SemanticVersion version;

  std::string GetCPMSuffix() const;
};


enum class RepositoryType {
  OTHER,
  GITHUB,
  GITLAB,
  BITBUCKET,
};

struct Repository {
  RepositoryType type;
  std::string uri;
  std::string owner;
  std::string name;

  static std::optional<Repository> Parse(std::string_view uri);

  // Returns the list of available versions sorted from oldest to newest.
  std::vector<TaggedVersion> QueryVersions() const;

  // Returns the latest version of the package.
  std::optional<TaggedVersion> QueryLatestVersion() const;

  // Returns the definition for a CPMAddPackge call.
  std::string GetCPMDefinition(const std::optional<TaggedVersion>& version = std::nullopt) const;

  // Returns the CPMAddPackge definition for the latest version
  std::string GetCPMDefinitionForLatestVersion() const;
};

struct RegisteredPackage {
  Repository repository;

  static std::optional<RegisteredPackage> Parse(const nlohmann::json& json);

};

std::optional<RegisteredPackage> FindPackage(std::string_view package_name);
