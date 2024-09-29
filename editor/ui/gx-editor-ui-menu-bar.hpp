#ifndef GEAROENIX_EDITOR_UI_MENU_BAR_HPP
#define GEAROENIX_EDITOR_UI_MENU_BAR_HPP
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::control {
struct Manager;
}

namespace gearoenix::editor::ui {
struct Manager;
struct MenuWindow;
struct MenuBar final {
private:
    platform::Application& platform_application;
    control::Manager& control_manager;
    Manager& manager;

    GX_GET_UPTR_PRV(MenuWindow, window);

    bool show_project_new_popup = false;
    bool show_scene_import_popup = false;

    void show_project();
    void show_scene();

public:
    MenuBar(platform::Application& platform_application, control::Manager& control_manager, Manager& manager);
    ~MenuBar();
    void update();
};
}

#endif