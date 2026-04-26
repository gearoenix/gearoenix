#pragma once
#include <gearoenix/core/gx-cr-object-type-indices.hpp>

#include <ImGui/imgui.h>

namespace gearoenix::core::ecs {
struct Entity;
}

/// \file
/// Material-Icons-font utilities and icon-aware text widgets used by the editor UI.
///
/// The editor renders component/entity badges using a small subset of Google's Material
/// Icons font (embedded at `assets/material-icons.ttf`) merged into ImGui's default font.
/// Icons are therefore rendered as regular text glyphs, which means:
///   * they inherit the current font size (so the UI scale slider works automatically),
///   * they can be coloured independently by overdrawing them via `ImDrawList::AddText`.
///
/// Each component type in the engine has a corresponding `ICON_*` glyph and an accent
/// colour; an entity's "primary" component (the most visually meaningful role it plays)
/// is picked with `pick_primary_component_type`.

namespace gearoenix::editor::ui {
// -------------------------------------------------------------------------------------
//  Material-Icons glyph constants (pre-encoded UTF-8 for codepoints in U+E000..U+F8FF).
// -------------------------------------------------------------------------------------
constexpr auto ICON_WORLD = "\xEE\xA0\x8B"; // public
constexpr auto ICON_CAMERA = "\xEE\x90\x92"; // photo_camera
constexpr auto ICON_LIGHT = "\xEE\x83\xB0"; // lightbulb
constexpr auto ICON_LIGHT_DIR = "\xEE\x90\xB0"; // wb_sunny
constexpr auto ICON_LIGHT_POINT = "\xEE\x94\x98"; // light_mode
constexpr auto ICON_MODEL = "\xEE\xA7\xBE"; // view_in_ar
constexpr auto ICON_MESH = "\xEE\x8F\xAC"; // grid_on
constexpr auto ICON_SKYBOX = "\xEE\x8A\xBD"; // cloud
constexpr auto ICON_MATERIAL = "\xEE\x90\x8A"; // palette
constexpr auto ICON_TRANSFORM = "\xEE\xA2\x9F"; // open_with
constexpr auto ICON_REFLECTION = "\xEE\x8E\xA2"; // blur_circular
constexpr auto ICON_CONSTRAINT = "\xEE\x85\x97"; // link
constexpr auto ICON_ANIMATION = "\xEE\x80\xB8"; // play_circle_filled
constexpr auto ICON_COLLIDER = "\xEE\x8F\x82"; // crop_free
constexpr auto ICON_GENERIC = "\xEE\xA1\xBB"; // extension

constexpr auto ICON_POP_OUT = "\xEE\xA2\x9E"; // open_in_new
constexpr auto ICON_DOCK = "\xEF\x87\x8F"; // close_fullscreen
constexpr auto ICON_SEARCH = "\xEE\xA2\xB6"; // search
constexpr auto ICON_VISIBLE = "\xEE\xA3\xB4"; // visibility
constexpr auto ICON_HIDDEN = "\xEE\xA3\xB5"; // visibility_off
constexpr auto ICON_CLOSE = "\xEE\x97\x8D"; // close
constexpr auto ICON_CHEVRON_LEFT = "\xEE\x97\x8B"; // chevron_left
constexpr auto ICON_CHEVRON_RIGHT = "\xEE\x97\x8C"; // chevron_right
constexpr auto ICON_EXPAND_MORE = "\xEE\x97\x8F"; // expand_more
constexpr auto ICON_SETTINGS = "\xEE\xA2\xB8"; // settings
constexpr auto ICON_MORE_VERT = "\xEE\x97\x94"; // more_vert

// -------------------------------------------------------------------------------------
//  Font loading
// -------------------------------------------------------------------------------------

/// Merge the Material-Icons TTF into the current default ImGui font so icon glyphs
/// render inline with text. Idempotent — safe to call every frame. Returns `true` on
/// success (font atlas updated) and `false` if the font asset was missing or unreadable.
bool load_icon_font();

// -------------------------------------------------------------------------------------
//  Component / entity → icon & colour lookup
// -------------------------------------------------------------------------------------

/// The "representative" component type for this entity, picked by a fixed priority list
/// (Scene > Camera > Light > … > Transform). Returns `gearoenix_core_ecs_entity_type_index`
/// when the entity is null or has no recognised component.
[[nodiscard]] core::object_type_index_t pick_primary_component_type(const core::ecs::Entity* entity);

/// Material-Icons glyph for the given component type (or `ICON_GENERIC` as a fallback).
[[nodiscard]] const char* icon_for_component_type(core::object_type_index_t type);

/// Accent colour (packed ImU32) tinting the icon for the given component type. Colours
/// are designed to be light, low-saturation greyish hues that read as a legend on a
/// dark theme.
[[nodiscard]] ImU32 icon_colour_for_component_type(core::object_type_index_t type);

/// Convenience: `icon_for_component_type(pick_primary_component_type(entity))`.
[[nodiscard]] const char* icon_for_entity(const core::ecs::Entity* entity);

/// Convenience: `icon_colour_for_component_type(pick_primary_component_type(entity))`.
[[nodiscard]] ImU32 icon_colour_for_entity(const core::ecs::Entity* entity);

/// Strip the leading `gearoenix::…::` namespaces from the registered type name so
/// component tabs and labels read as `"Scene"` / `"Model"` / `"Transform"` instead of
/// the fully qualified class name. Returns a pointer into the persistent type-name
/// string held by the object registry — no allocation, valid for the program lifetime.
[[nodiscard]] const char* short_component_type_name(core::object_type_index_t type);

// -------------------------------------------------------------------------------------
//  Icon-aware widget helpers
// -------------------------------------------------------------------------------------

/// Render `ImGui::TreeNodeEx` whose label visually reads `"<coloured-icon> <name>"`.
/// The icon is drawn via the window draw list with its own colour, so the tree node's
/// selection / hover highlight doesn't tint it. Returns the same boolean as
/// `TreeNodeEx` (whether the node is open).
///
/// The label is formatted into a small stack buffer inside the function, so there is
/// no heap allocation per frame. `display_name` must be a caller-owned null-terminated
/// C-string.
///
/// @param id            ImGui id string (the part after `##`, or the whole thing if it contains it)
/// @param flags         regular `ImGuiTreeNodeFlags`
/// @param icon          UTF-8 Material-Icons glyph to draw at the start of the label
/// @param icon_colour   ImU32 accent colour for the icon
/// @param display_name  human-readable name shown after the icon
[[nodiscard]] bool tree_node_with_icon(
    const char* id,
    ImGuiTreeNodeFlags flags,
    const char* icon,
    ImU32 icon_colour,
    const char* display_name);

/// Render `ImGui::BeginTabItem` with an inline-coloured icon, using the same overdrawn
/// technique as `tree_node_with_icon`. The returned value is the raw `BeginTabItem`
/// result; the caller is still responsible for calling `EndTabItem()` on success.
///
/// Like `tree_node_with_icon`, formats the label into a stack buffer — no heap
/// allocation per frame.
///
/// @param id_suffix     a disambiguating id (e.g. `"42"`); the final tab id is
///                      `"<padding><display_name>##<id_suffix>"`
/// @param icon          UTF-8 Material-Icons glyph to draw at the start of the label
/// @param icon_colour   ImU32 accent colour for the icon
/// @param display_name  human-readable name shown after the icon
[[nodiscard]] bool tab_item_with_icon(
    const char* id_suffix,
    const char* icon,
    ImU32 icon_colour,
    const char* display_name);
}
