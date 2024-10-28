#ifndef GEAROENIX_EDITOR_UI_MENU_BAR_HPP
#define GEAROENIX_EDITOR_UI_MENU_BAR_HPP
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>
#include <memory>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::editor::ui {
struct Manager;
struct MenuEntity;
struct MenuProject;
struct MenuWindow;
struct MenuBar final {
private:
    Manager& manager;

    GX_GET_UPTR_PRV(MenuEntity, entity_menu);
    GX_GET_UPTR_PRV(MenuProject, project_menu);
    GX_GET_UPTR_PRV(MenuWindow, window_menu);

    bool show_scene_import_popup = false;

    void show_scene();

public:
    explicit MenuBar(Manager& manager);
    ~MenuBar();
    void update();
};
}

#endif