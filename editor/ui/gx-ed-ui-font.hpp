#pragma once

namespace gearoenix::editor::ui::font {
/// Editor-wide UI font size in pixels — the single knob that drives every
/// text-derived metric (widget heights, icon sizes, viewport-button geometry,
/// gizmo handles). `EditorApplication::update` reads it and pushes it once per
/// frame via `ImGui::PushFont(nullptr, …)` so the whole editor rescales in lockstep.
///
/// Constants are top-level `constexpr`s rather than members hidden behind static
/// getters — they're values, not behaviour, so callers reference them directly:
/// `ui::font::min_size_px`, `ui::font::default_size_px`, etc.
constexpr float min_size_px = 10.0f;
constexpr float max_size_px = 28.0f;
constexpr float default_size_px = 18.0f;

/// Current size, clamped on write to `[min_size_px, max_size_px]`.
[[nodiscard]] float get_size_px();
void set_size_px(float size);
}