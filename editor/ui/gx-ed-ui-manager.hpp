#pragma once
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::editor::ui {
struct MenuBar;
struct WindowOverlayProgressBarManager;
struct Manager final {
private:
    GX_GET_UPTR_PRV(MenuBar, menu_bar);
    GX_GET_UPTR_PRV(WindowOverlayProgressBarManager, window_overlay_progress_bar_manager);

public:
    Manager();
    ~Manager();
    void update();
    void renew();
};
}