#pragma once

#include <vector>
#include "utils.hpp"

struct Context {
  struct {
    Path home;
    Path config;
    std::vector<Path> registries;
    Path cmake;
  } paths;
} extern g_context;

void InitContext();
