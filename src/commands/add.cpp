#include "../commands.hpp"
#include "../utils.hpp"
#include "../project.hpp"

void AddAddCommand(CLI::App& app) {
  const auto add_command = app.add_subcommand("add", "Adds a package or component to the project");

  {
    const auto add_library_command = add_command->add_subcommand("library");

    static std::string library_name;

    add_library_command
      ->add_option("library_name", library_name)
      ->description("The name of the library")
      ->required();

    add_library_command->callback(
      [&]() {
        const auto project = Project::Open(fs::current_path());
        if (project) {
          project->AddTarget(library_name, TargetType::LIBRARY);
        }
      }
    );
  }

  {
    const auto add_executable_command = add_command->add_subcommand("executable");

    static std::string executable_name;

    add_executable_command
      ->add_option("executable_name", executable_name)
      ->description("The name of the executable")
      ->required();

    add_executable_command->callback(
      [&]() {
        const auto project = Project::Open(fs::current_path());
        if (project) {
          project->AddTarget(executable_name, TargetType::LIBRARY);
        }
      }
    );
  }

  AddAddPackageCommand(*add_command);
}
