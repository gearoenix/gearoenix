#include "gx-editor-ui-manager.hpp"

gearoenix::editor::ui::Manager::Manager(
        platform::Application& platform_application,
        editor::control::Manager& control_manager) noexcept
        : platform_application(platform_application)
        , control_manager(control_manager)
        , menu_bar(platform_application, control_manager)
{
}

void gearoenix::editor::ui::Manager::update() noexcept {
    menu_bar.update();
}
