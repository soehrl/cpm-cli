#include "../commands.hpp"
#include "../utils.hpp"
#include "../project.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include "subprocess.hpp"

void AddBuildCommand(CLI::App& app) {
  const auto configure_command = app.add_subcommand("build", "Builds the project");

  static std::string build_type;

  configure_command
    ->add_option("build_type", build_type)
    ->description("The build type debug|release");

  configure_command->callback([&]() {
    const auto project = Project::Open(fs::current_path());
    if (!project) {
      return;
    }

    const auto build_path = project->path / "build";
    if (!fs::exists(build_path)) {
      spdlog::info("Project has not been configured yet");
      if (!fs::create_directory(build_path)) {
        spdlog::error("Failed to create build directory {}", build_path.string());
        return;
      }
      const auto configure_result = subprocess::call({ "cmake", ".." }, subprocess::cwd{ build_path.string().c_str() });
      if (configure_result) {
        spdlog::error("Failed to configure project");
        return;
      }
    }

    const auto configure_result = subprocess::call({ "cmake", "--build", "." }, subprocess::cwd{ build_path.string().c_str() });
    if (configure_result) {
      spdlog::error("Failed to build project");
      return;
    }
  });
}
