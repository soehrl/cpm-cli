#include "../commands.hpp"
#include "../utils.hpp"
#include "../project.hpp"

void AddAddCommand(CLI::App& app) {
  const auto add_command = app.add_subcommand("add", "Adds additional packages to the project");

  static std::string package_definition;

  add_command
    ->add_option("package_name", package_definition)
    ->description("The identifier of the package")
    ->required();

  add_command->callback(
    [&]() {
      const auto project = Project::Open(fs::current_path());
      if (project) {
        project->AddPackage(package_definition);
      }
    }
  );
}
