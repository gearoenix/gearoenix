#include "gx-ed-ui-window-scene.hpp"

gearoenix::editor::ui::WindowScene::WindowScene(Manager& manager)
    : platform_application(platform_application)
    , control_manager(control_manager)
    , manager(manager)
{
}

void gearoenix::editor::ui::WindowScene::update()
{
}

bool* gearoenix::editor::ui::WindowScene::get_show_window_ptr()
{
    return &show_window;
}
