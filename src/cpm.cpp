#include "commands.hpp"
#include "cmake.hpp"
#include "spdlog/spdlog.h"
#include "CLI/CLI.hpp"

int main(int argc, char* argv[]) {
  // FindCMake();

  CLI::App app;

  AddCreateCommand(app);
  AddAddCommand(app);
  AddConfigureCommand(app);
  AddBuildCommand(app);
  app.require_subcommand();

  // app.validate_positionals();

  CLI11_PARSE(app, argc, argv);
}
