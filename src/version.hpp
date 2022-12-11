#pragma once

#include <optional>
#include <string>

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
