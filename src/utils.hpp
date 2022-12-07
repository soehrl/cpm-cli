#pragma once

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

using Path = fs::path;

std::optional<std::string> ReadFile(const Path& path);
bool WriteFile(const Path& path, std::string_view content);
bool AppendFile(const Path& path, std::string_view content);
