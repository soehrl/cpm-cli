#include "context.hpp"

#include <cstdlib>

Context g_context;

void InitContext() {
  g_context.paths.home = std::getenv("HOME");
  g_context.paths.config = g_context.paths.home / ".local" / "share" / "cpm-cli";
  g_context.paths.registries.push_back(g_context.paths.config / "registries" / "cpm-cli");
}
