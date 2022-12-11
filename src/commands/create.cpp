#include "../commands.hpp"
#include "../utils.hpp"
#include "../project.hpp"
#include "CLI/Error.hpp"

void AddCreateCommand(CLI::App& app) {
  const auto create_command = app.add_subcommand("create", "Create a new project");

  static std::string project_name;

  static std::optional<std::string> template_uri;
  create_command
    ->add_option("-t,--template", template_uri)
    ->description("Git repository containing the template for the project");

  create_command
    ->add_option("project_name", project_name)
    ->description("The name of the project")
    ->required();

  create_command->callback([&]() {
    const auto project = Project::Create(fs::current_path() / project_name, template_uri ? *template_uri : "");
    if (!project) {
      throw CLI::RuntimeError(-1);
    }
  });
}
