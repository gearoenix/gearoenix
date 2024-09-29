#ifndef GEAROENIX_EDITOR_UI_MENU_BAR_HPP
#define GEAROENIX_EDITOR_UI_MENU_BAR_HPP

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
    control::Manager& control_manager;

    bool show_project_new_popup = false;
    bool show_scene_import_popup = false;

    void show_project();
    void show_scene();

public:
    MenuBar(platform::Application& platform_application, control::Manager& control_manager);
    void update();
};
}

#endif