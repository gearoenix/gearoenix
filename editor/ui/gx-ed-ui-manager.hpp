#ifndef GEAROENIX_EDITOR_UI_MANAGER_HPP
#define GEAROENIX_EDITOR_UI_MANAGER_HPP
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::ui {
struct MenuBar;
struct WindowScene;
struct WindowOverlayProgressBarManager;
struct Manager final {
private:
    GX_GET_RRF_PRV(platform::Application, platform_application);
    GX_GET_UPTR_PRV(MenuBar, menu_bar);
    GX_GET_UPTR_PRV(WindowScene, window_scene);
    GX_GET_UPTR_PRV(WindowOverlayProgressBarManager, window_overlay_progree_bar_manager);

public:
    explicit Manager(platform::Application& platform_application);
    ~Manager();
    void update();
};
}

#endif