#include "gx-editor-ui-manager.hpp"
#include "gx-editor-ui-menu-bar.hpp"
#include "gx-editor-ui-window-overlay-progress-bar.hpp"
#include "gx-editor-ui-window-scene.hpp"

gearoenix::editor::ui::Manager::Manager(
    platform::Application& platform_application,
    control::Manager& control_manager)
    : platform_application(platform_application)
    , control_manager(control_manager)
    , menu_bar(new MenuBar(platform_application, control_manager, *this))
    , window_scene(new WindowScene(platform_application, control_manager, *this))
    , window_overlay_progree_bar_manager(new WindowOverlayProgressBarManager())
{
}

gearoenix::editor::ui::Manager::~Manager() = default;

void gearoenix::editor::ui::Manager::update()
{
    menu_bar->update();
    window_overlay_progree_bar_manager->update();
}
