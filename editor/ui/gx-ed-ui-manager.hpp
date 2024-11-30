#pragma once
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::ui {
struct MenuBar;
struct WindowOverlayProgressBarManager;
struct Manager final {
private:
    GX_GET_RRF_PRV(platform::Application, platform_application);
    GX_GET_UPTR_PRV(MenuBar, menu_bar);
    GX_GET_UPTR_PRV(WindowOverlayProgressBarManager, window_overlay_progree_bar_manager);

public:
    explicit Manager(platform::Application& platform_application);
    ~Manager();
    void update();
};
}