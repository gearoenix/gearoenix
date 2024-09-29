#ifndef GEAROENIX_EDITOR_UI_MANAGER_HPP
#define GEAROENIX_EDITOR_UI_MANAGER_HPP
#include "gx-editor-ui-menu-bar.hpp"

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::control {
struct Manager;
}

namespace gearoenix::editor::ui {
struct Manager final {
private:
    platform::Application& platform_application;
    control::Manager& control_manager;
    MenuBar menu_bar;

public:
    Manager(platform::Application& platform_application, control::Manager& control_manager);
    void update();
};
}

#endif