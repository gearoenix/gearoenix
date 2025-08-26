#pragma once
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::editor::ui {
struct MenuEntity;
struct MenuWorld;
struct MenuScene;
struct MenuWindow;
struct MenuBar final {
private:
    GX_GET_UPTR_PRV(MenuEntity, entity_menu);
    GX_GET_UPTR_PRV(MenuWorld, project_menu);
    GX_GET_UPTR_PRV(MenuScene, scene_menu);
    GX_GET_UPTR_PRV(MenuWindow, window_menu);

    bool show_scene_import_popup = false;

public:
    MenuBar();
    ~MenuBar();
    void update();
    void renew();
};
}