#include "gx-editor-ctrl-project.hpp"
#include "gx-editor-ctrl-manager.hpp"

gearoenix::editor::control::Project::Project(
        platform::Application& platform_application,
        Manager& manager) noexcept
: project(std::nullopt)
, platform_application(platform_application)
, manager(manager)
{
}

void gearoenix::editor::control::Project::create_project() noexcept {
    project.emplace(project_name);
    manager.caption_changed();
}

void gearoenix::editor::control::Project::caption_changed() noexcept {
    manager.caption_changed();
}

std::string gearoenix::editor::control::Project::get_caption() noexcept {
    return " [ " + (project.has_value()? "Project: " + project->get_name() : "No project") + " ]";
}

bool gearoenix::editor::control::Project::is_project_opened() noexcept {
    return project.has_value();
}
