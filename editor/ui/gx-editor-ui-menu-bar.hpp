#ifndef GEAROENIX_EDITOR_UI_MENU_BAR_HPP
#define GEAROENIX_EDITOR_UI_MENU_BAR_HPP

#include <string>

namespace gearoenix::platform {
    struct Application;
}

namespace gearoenix::editor::control {
    struct Manager;
}

namespace gearoenix::editor::ui {
struct MenuBar final {
private:
    platform::Application& platform_application;
    editor::control::Manager& control_manager;

    bool show_project_new_popup = false;
    bool show_scene_import_popup = false;

    void show_project() noexcept;
    void show_scene() noexcept;
public:
    explicit MenuBar(platform::Application& platform_application, editor::control::Manager& control_manager) noexcept;
    void update() noexcept;
};
}

#endif