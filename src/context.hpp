#pragma once

#include <toml++/toml.h>
#include <vector>
#include "utils.hpp"

struct Context {
  struct {
    Path home;
    Path config_directory;
    Path config_file;
    Path cache;
    Path registries;
    Path cpm_cache;
    Path cmake;
  } paths;

  toml::table config;

  static bool Init();

  bool SetupRegistries() const;
} extern g_context;
