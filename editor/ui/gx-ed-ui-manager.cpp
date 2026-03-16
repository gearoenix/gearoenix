#include "gx-ed-ui-manager.hpp"
#include "gx-ed-ui-menu-bar.hpp"
#include "gx-ed-ui-window-overlay-progress-bar.hpp"

gearoenix::editor::ui::Manager::Manager()
    : menu_bar(new MenuBar())
    , window_overlay_progress_bar_manager(new WindowOverlayProgressBarManager())
{
}

gearoenix::editor::ui::Manager::~Manager() = default;

void gearoenix::editor::ui::Manager::update()
{
    menu_bar->update();
    window_overlay_progress_bar_manager->update();
}

void gearoenix::editor::ui::Manager::renew() { menu_bar->renew(); }
