#pragma once
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::ui {
struct Manager;
struct MenuEntity;
struct MenuWorld;
struct MenuScene;
struct MenuWindow;
struct MenuBar final {
private:
    Manager& manager;

    GX_GET_UPTR_PRV(MenuEntity, entity_menu);
    GX_GET_UPTR_PRV(MenuWorld, project_menu);
    GX_GET_UPTR_PRV(MenuScene, scene_menu);
    GX_GET_UPTR_PRV(MenuWindow, window_menu);

    bool show_scene_import_popup = false;

public:
    explicit MenuBar(Manager& manager);
    ~MenuBar();
    void update();
};
}