#include <fstream>
#include <ios>

#include "utils.hpp"

std::optional<std::string> ReadFile(const Path& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return std::nullopt;
  }

  file.seekg(0, std::ios::end);
  std::string string(file.tellg(), '\0');
  file.seekg(0, std::ios::beg);
  if (!file.read(string.data(), string.size())) {
    return std::nullopt;
  }

  return string;
}

bool WriteFile(const Path& path, std::string_view content) {
  std::ofstream file(path);
  file.write(content.data(), content.size());
  return file.good();
}

bool AppendFile(const Path& path, std::string_view content) {
  std::ofstream file(path, std::ios::app);
  file.write(content.data(), content.size());
  return false;
}
