#include "version.hpp"

#include <regex>
#include "spdlog/fmt/bundled/format.h"

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
