#include "cmake.hpp"
#include "spdlog/spdlog.h"
#include "subprocess.hpp"

bool FindCMake() {
  namespace sp = subprocess;

  try {
    const auto cmake = sp::check_output({ "cmake", "--version" });
    const std::string_view cmake_version(cmake.buf.data(), cmake.length);

    spdlog::info("Found CMake version {}", cmake_version);

    return true;
  } catch (...) {
    spdlog::error("Cannot find CMake");
    return false;
  }
}
