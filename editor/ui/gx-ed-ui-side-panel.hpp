#pragma once

#include <gearoenix/core/gx-cr-singleton.hpp>

#include <ImGui/imgui.h>

namespace gearoenix::editor::ui {
struct DockableSection;

/// Right-hand dock. Hosts exactly two `DockableSection`s stacked
/// vertically with a drag-splitter between them, plus a drag-handle on the left
/// edge that resizes the whole panel. Either section can be popped out into its
/// own floating window via the header's dock-toggle button — the `SidePanel` then
/// renders it as a free-floating ImGui window for as long as it stays detached.
///
/// The panel owns the layout geometry (width ratio, splitter position).
struct SidePanel final : core::Singleton<SidePanel> {
private:
    /// Whole-panel visibility toggle, independent of each section's own `shown` flag.
    /// When false, the panel is skipped entirely (including any floating window spawned
    /// by popping a section out) — the viewport gets the full horizontal space.
    /// Exposed to the Window menu as a bool reference so the user can toggle it.
    bool shown = true;

    // -- Side-panel geometry (ratios of the main viewport) -----------------------------
    float width_ratio = 0.22f;
    float first_section_height_ratio = 0.52f;

    /// Cached panel width from the last `update()` pass. Zero when no section is docked
    /// (a panel is skipped entirely and the viewport gets the full width). `Viewport`
    /// reads this to know how much horizontal space it can use for the 3D area.
    float cached_width = 0.0f;

    void draw_sections_splitter(float current_first_section_h, float full_h, float thickness_px);
    void draw_panel_resize_handle(float panel_width, float work_w);
    void draw_sections_column();
    void draw_docked_panel();
    /// Small "show panel" handle drawn against the right edge while `shown == false`.
    /// Clicking it flips `shown` back on and bumps `width_ratio` so the panel is at
    /// least the minimum width on re-appearance.
    void draw_nudge_button();

public:
    SidePanel();
    ~SidePanel() override = default;

    /// Drive one frame of the side panel. Draws the docked panel (if any section is
    /// attached) plus any free-floating windows for detached sections.
    void update();

    /// Width of the panel computed during the last `update()`, or 0 when nothing is docked.
    [[nodiscard]] float get_width() const { return cached_width; }

    /// Reference to the whole-panel "shown" flag — wired up by the Window menu as a
    /// `bool*` that the user can toggle.
    [[nodiscard]] bool& get_shown() { return shown; }
};
}