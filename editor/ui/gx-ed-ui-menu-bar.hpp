#pragma once
#include <gearoenix/core/gx-cr-singleton.hpp>

#include <memory>

namespace gearoenix::editor::ui {
struct MenuEntity;
struct MenuWorld;
struct MenuScene;
struct MenuWindow;

struct MenuBar final : core::Singleton<MenuBar> {
    const std::unique_ptr<MenuEntity> entity_menu;
    const std::unique_ptr<MenuWorld> project_menu;
    const std::unique_ptr<MenuScene> scene_menu;
    const std::unique_ptr<MenuWindow> window_menu;

    MenuBar();
    ~MenuBar() override;

    void update();
    void renew();
};
}