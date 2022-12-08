#include "../commands.hpp"
#include "../utils.hpp"
#include "../project.hpp"
#include "CLI/Error.hpp"

void AddCreateCommand(CLI::App& app) {
  const auto create_command = app.add_subcommand("create", "Create a new project");

  static std::string project_name;

  create_command
    ->add_option("project_name", project_name)
    ->description("The name of the project")
    ->required();

  create_command->callback([&]() {
    const auto project = Project::Create(fs::current_path(), project_name);
    if (!project) {
      throw CLI::RuntimeError(-1);
    }
    const auto default_target = project->AddTarget(project_name, TargetType::EXECUTABLE);
  });
}
