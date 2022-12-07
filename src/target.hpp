#pragma once

#include <memory>
#include "utils.hpp"

class Project;

enum class TargetType {
  NONE,
  EXECUTABLE,
  LIBRARY,
};

class Target {
  friend class Project;

 public:
  Target() = delete;

 private:
  Target(std::shared_ptr<Project> project, Path path, std::string name, TargetType type);

  static std::shared_ptr<Target> Create(std::shared_ptr<Project> project, std::string name, TargetType type);
  static std::shared_ptr<Target> OpenTarget(std::shared_ptr<Project> project, std::string name);

  std::shared_ptr<Project> m_project;
  Path m_path;
  std::string m_name;
  TargetType m_type;
};

// struct CreateTargetParameters {
//   Path project_path;
//   std::string target_name;
//   TargetType type;
// };

// void CreateTarget(const CreateTargetParameters& parameters);
