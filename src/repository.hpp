#pragma once

#include <optional>
#include <string>

#include "version.hpp"

enum class RepositoryType {
  OTHER,
  GITHUB,
  GITLAB,
  BITBUCKET,
};

struct Repository {
  RepositoryType type;
  std::string url;
  std::string owner;
  std::string name;

  static std::optional<Repository> Parse(std::string_view url);

  // Returns the list of available versions sorted from oldest to newest.
  std::vector<TaggedVersion> QueryVersions(std::string_view version_prefix = "") const;

  // Returns the latest version of the package.
  std::optional<TaggedVersion> QueryLatestVersion(std::string_view version_prefix = "") const;

  // Returns the definition for a CPMAddPackge call.
  std::string GetCPMDefinition(const std::optional<TaggedVersion>& version = std::nullopt) const;

  // Returns the CPMAddPackge definition for the latest version
  std::string GetCPMDefinitionForLatestVersion(std::string_view version_prefix = "") const;
};

