#pragma once
#include <gearoenix/core/macro/gx-cr-mcr-getter-setter.hpp>

#include <ImGui/imgui.h>

#include <memory>
#include <string>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::editor::viewport {
/// Base class for the floating 3D-viewport toolbar buttons (projection toggle,
/// translate/rotate/scale gizmo toggles, …).
///
/// Each button is its own transparent ImGui window drawn at an absolute position in
/// the editor's 3D area. The base class owns all visual metrics (height, icon size,
/// paddings, rounding, label font) and the full draw routine (`show()`); subclasses
/// only have to say **where** the button sits on the row and **how wide** it is by
/// overriding `compute_start_x()` and `compute_size()`. `apply_current_scale()` is
/// called every frame, so the row re-anchors to the menu bar and rescales cleanly
/// whenever the editor's UI text size changes.
struct Button {
    const std::string invisible_window_id, invisible_button_id;

    GX_GET_CREF_PRT(ImVec2, start_point);
    GX_GET_CREF_PRT(ImVec2, size);
    GX_GET_CREF_PRT(ImVec2, end_point);
    GX_GET_CREF_PRT(ImVec2, icon_start_point);
    GX_GET_CREF_PRT(ImVec2, icon_end_point);
    GX_GET_CREF_PRT(ImVec2, text_start_point);
    GX_GET_CREF_PRT(std::string, text);
    GX_GET_CREF_PRT(std::string, toggled_text);
    GX_GET_CREF_PRT(std::shared_ptr<render::texture::Texture2D>, icon);
    GX_GET_CREF_PRT(std::shared_ptr<render::texture::Texture2D>, toggled_icon);
    GX_GET_CPTR_PRT(char, tooltip);
    GX_GET_CPTR_PRT(char, toggled_tooltip);
    GX_GET_VAL_PRT(bool, toggled, false);
    GX_GET_VAL_PRT(bool, is_toggleable, false);
    GX_GET_VAL_PRT(bool, clicked_in_this_frame, false);
    /// Visual metrics — all recomputed every frame in `apply_current_scale()` so they
    /// follow `ImGui::GetFontSize()` / the menu-bar height. The defaults below are only
    /// used for the first frame before the Button's `show()` runs.
    GX_GET_VAL_PRT(float, icon_size, 20.0f);
    GX_GET_VAL_PRT(float, height, 28.0f);
    GX_GET_VAL_PRT(float, icon_margin, 4.0f);
    GX_GET_VAL_PRT(float, icon_end, 24.0f);
    GX_GET_VAL_PRT(float, top_margin, 30.0f);
    GX_GET_VAL_PRT(float, spacing, 10.0f);
    GX_GET_VAL_PRT(float, text_font_size, 15.0f);
    GX_GET_VAL_PRT(float, rounding, 10.0f);
    GX_GET_VAL_PRT(ImU32, text_colour, IM_COL32(200, 200, 200, 255));
    GX_GET_VAL_PRT(ImU32, background_colour, IM_COL32(80, 80, 80, 200));
    GX_GET_VAL_PRT(ImU32, toggled_background_colour, background_colour);
    GX_GET_VAL_PRT(ImU32, border_colour, IM_COL32(128, 128, 128, 200));

    /// Font size at which the hard-coded unit values below (20, 28, 30, 10, …)
    /// were authored. All per-frame scaling is `ImGui::GetFontSize() / reference_font_size`.
    static constexpr float reference_font_size = 15.0f;

protected:
    // ---- Required by subclasses: one-line layout hooks ---------------------------
    /// @return The X coordinate (in absolute ImGui display-space) of this button's left edge.
    /// For the first button on the row it's typically `spacing`; later buttons
    /// return `previous.get_end_point().x + spacing`.
    [[nodiscard]] virtual float compute_start_x() const = 0;
    /// @return the button's `{width, height}` in pixels.
    [[nodiscard]] virtual ImVec2 compute_size() const = 0;

private:
    /// Recompute every scale-dependent metric (font size, icon size, row height,
    /// paddings, row anchor) based on the current `ImGui::GetFontSize()` and the main
    /// viewport's menu-bar height.
    void apply_current_scale();
    /// Derive the four internal rectangles (`end_point`, `icon_*`, `text_start_point`)
    /// from `start_point` + `size` + paddings.
    void compute_values();

public:
    explicit Button();
    virtual ~Button();

    /// Render the button — applies scale, positions itself via the subclass hooks and
    /// then draws the window, background, icon, label and handles the click. Subclass
    /// `update()` methods should call this and then react to `clicked_in_this_frame`.
    void show();

    virtual void update() = 0;
};
}
