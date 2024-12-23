#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-menu-bar.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

gearoenix::editor::ui::Manager::Manager(platform::Application& platform_application)
    : platform_application(platform_application)
    , menu_bar(new MenuBar(*this))
    , window_overlay_progree_bar_manager(new WindowOverlayProgressBarManager())
{
}

gearoenix::editor::ui::Manager::~Manager() = default;

void gearoenix::editor::ui::Manager::update()
{
    menu_bar->update();
    window_overlay_progree_bar_manager->update();
}
