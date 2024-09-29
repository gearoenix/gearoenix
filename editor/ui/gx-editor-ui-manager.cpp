#include "gx-editor-ui-manager.hpp"

gearoenix::editor::ui::Manager::Manager(
    platform::Application& platform_application,
    control::Manager& control_manager)
    : platform_application(platform_application)
    , control_manager(control_manager)
    , menu_bar(platform_application, control_manager)
{
}

void gearoenix::editor::ui::Manager::update()
{
    menu_bar.update();
}
