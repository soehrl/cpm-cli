#include "../commands.hpp"
#include "../project.hpp"

void AddAddPackageCommand(CLI::App& add_command) {
  const auto add_package_command = add_command.add_subcommand("package");

  static std::string package_definition;

  add_package_command
    ->add_option("package_name", package_definition)
    ->description("The identifier of the package")
    ->required();

  add_package_command->callback(
    [&]() {
      const auto project = Project::Open(fs::current_path());
      if (project) {
        project->AddPackage(package_definition);
      }
    }
  );
}
