#include "../commands.hpp"
#include "../utils.hpp"
#include "../project.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include "subprocess.hpp"

void AddConfigureCommand(CLI::App& app) {
  const auto configure_command = app.add_subcommand("configure", "Configures the cmake project");

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
    if (!fs::exists(build_path) && !fs::create_directory(build_path)) {
      spdlog::error("Failed to create build directory {}", build_path.string());
      return;
    }

    const auto configure = subprocess::check_output({ "cmake", ".." }, subprocess::cwd{ build_path.string().c_str() });
    const std::string_view configure_output(configure.buf.data(), configure.length);

    fmt::print("{}", configure_output);
  });
}
