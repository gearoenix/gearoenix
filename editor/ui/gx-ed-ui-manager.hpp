#pragma once
#include <gearoenix/core/gx-cr-singleton.hpp>

#include <memory>

namespace gearoenix::editor::ui {
struct MenuBar;
struct Entities;
struct Components;
struct SidePanel;
struct WindowOverlayProgressBarManager;

/// Top-level UI manager owned by `EditorApplication`. Composes the menu bar, the
/// two dockable sections (`Entities` + `Components`), the `SidePanel` that hosts
/// them on the right-hand side, and overlay progress bars.
struct Manager final : core::Singleton<Manager> {
    const std::unique_ptr<MenuBar> menu_bar;
    const std::unique_ptr<Entities> entities;
    const std::unique_ptr<Components> components;
    const std::unique_ptr<SidePanel> side_panel;
    const std::unique_ptr<WindowOverlayProgressBarManager> window_overlay_progress_bar_manager;

    Manager();
    ~Manager() override;

    /// Drive one editor-UI frame. Called every frame by `EditorApplication::update`.
    void update();
    /// Propagate a "project was reloaded" event — tears down any transient menu state.
    void renew();
};
}