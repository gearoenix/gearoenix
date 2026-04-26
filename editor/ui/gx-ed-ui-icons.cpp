#include "gx-ed-ui-icons.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-entity.hpp>
#include <gearoenix/core/gx-cr-object.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-stream.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {
// ---------------------------------------------------------------------------
//  Priority order for picking an entity's "representative" component. The
//  earlier a type appears, the more visually meaningful it is as a badge.
//  Kept as a single source of truth so icon lookup and colour lookup agree.
// ---------------------------------------------------------------------------
constexpr std::array primary_priority {
    gearoenix_render_scene_type_index,
    gearoenix_render_camera_type_index,
    gearoenix_render_light_directional_type_index,
    gearoenix_render_light_point_type_index,
    gearoenix_render_light_shadow_caster_directional_type_index,
    gearoenix_render_light_type_index,
    gearoenix_render_skybox_type_index,
    gearoenix_render_reflection_probe_type_index,
    gearoenix_render_reflection_baked_type_index,
    gearoenix_render_reflection_runtime_type_index,
    gearoenix_render_model_type_index,
    gearoenix_render_mesh_type_index,
    gearoenix_physics_animation_player_type_index,
    gearoenix_physics_animation_armature_type_index,
    gearoenix_physics_animation_bone_type_index,
    gearoenix_physics_collider_type_index,
    gearoenix_physics_constraint_type_index,
    gearoenix_physics_transformation_type_index,
};

// Gate flag for `load_icon_font` idempotency.
bool g_icon_font_loaded = false;

// ---------------------------------------------------------------------------
//  Icon-overlay helpers — shared between tree nodes and tab items.
// ---------------------------------------------------------------------------

/// Shared scratch buffer used by `tree_node_with_icon` / `tab_item_with_icon` to format
/// their labels without per-frame heap allocations. Capacity is retained across calls
/// so after a few frames this is effectively zero-alloc regardless of label length.
/// Safe because the editor UI runs on a single thread and ImGui consumes the label
/// synchronously inside `TreeNodeEx` / `BeginTabItem`.
std::string g_label_scratch;

/// Fill `g_label_scratch` with `"<padding><display_name>"`, where the leading whitespace
/// is wide enough to fit `icon` plus a readable gap. The caller overdraws the real
/// (coloured) icon on top of that whitespace via `ImDrawList::AddText`.
void build_padded_label(const char* const icon, const char* const display_name)
{
    const float icon_w = ImGui::CalcTextSize(icon).x;
    const float space_w = std::max(1.0f, ImGui::CalcTextSize(" ").x);
    const float gap_px = ImGui::GetFontSize() * 0.60f;
    const int pad_count = std::max(0, static_cast<int>(std::ceil((icon_w + gap_px) / space_w)));

    g_label_scratch.clear();
    g_label_scratch.append(static_cast<std::size_t>(pad_count), ' ');
    if (nullptr != display_name) {
        g_label_scratch.append(display_name);
    }
}

/// Overdraw `icon` onto the most-recently-submitted ImGui item. The icon is placed
/// `x_offset` pixels from the item's left edge and vertically centred within the
/// item's row.
void overdraw_icon_on_last_item(const char* const icon, const ImU32 icon_colour, const float x_offset)
{
    const ImVec2 rect_min = ImGui::GetItemRectMin();
    const ImVec2 rect_max = ImGui::GetItemRectMax();
    const float row_h = rect_max.y - rect_min.y;
    const float icon_y = rect_min.y + (row_h - ImGui::GetFontSize()) * 0.5f;
    const float icon_x = rect_min.x + x_offset;
    ImGui::GetWindowDrawList()->AddText({ icon_x, icon_y }, icon_colour, icon);
}
}

// =====================================================================================
//  Font loading
// =====================================================================================

bool gearoenix::editor::ui::load_icon_font()
{
    if (g_icon_font_loaded) {
        return true;
    }

    const auto font_stream = platform::stream::Stream::open(platform::stream::Path::create_asset("material-icons.ttf"));
    if (nullptr == font_stream) {
        return false;
    }

    const auto size = font_stream->size();
    if (size < 1) {
        return false;
    }

    // The font buffer must outlive the atlas. We hand ImGui a non-owning pointer
    // (`FontDataOwnedByAtlas = false`) and keep ownership in this static so the
    // bytes stay alive for the program lifetime — RAII frees them on shutdown.
    static std::vector<std::uint8_t> data;
    data.resize(static_cast<std::size_t>(size));
    if (font_stream->read(data.data(), size) != size) {
        data.clear();
        data.shrink_to_fit();
        return false;
    }

    auto& io = ImGui::GetIO();
    // Match the default font's baked size so both fonts merge with consistent metrics;
    // ImGui 1.92's dynamic atlas rescales both together when the UI text size changes.
    const float base_size = io.FontDefault != nullptr ? io.FontDefault->LegacySize : 15.0f;

    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;
    cfg.MergeMode = true;
    cfg.PixelSnapH = true;
    // Square advance (icon == one em) so icons occupy the same footprint as a text glyph.
    cfg.GlyphMinAdvanceX = base_size;
    cfg.GlyphMaxAdvanceX = base_size;
    // Material-Icons glyphs are designed with their origin at the top of the em box;
    // nudge them down so they sit on the text baseline visually when mixed with text
    // (tree nodes, tab labels, etc.).
    constexpr float baseline_offset_ratio = 0.20f;
    cfg.GlyphOffset.y = base_size * baseline_offset_ratio;

    // Material-Icons glyphs all live in the Private Use Area.
    static const ImWchar icon_ranges[] = { 0xE000, 0xF8FF, 0 };

    io.Fonts->AddFontFromMemoryTTF(data.data(), static_cast<int>(size), base_size, &cfg, icon_ranges);

    g_icon_font_loaded = true;
    return true;
}

// =====================================================================================
//  Component/entity → icon & colour lookup
// =====================================================================================

gearoenix::core::object_type_index_t gearoenix::editor::ui::pick_primary_component_type(const core::ecs::Entity* const entity)
{
    if (nullptr == entity) {
        return gearoenix_core_ecs_entity_type_index;
    }
    const auto& comps = entity->get_all_types_to_components();
    for (const auto t : primary_priority) {
        if (comps.contains(t)) {
            return t;
        }
    }
    return gearoenix_core_ecs_entity_type_index;
}

const char* gearoenix::editor::ui::icon_for_component_type(const core::object_type_index_t type)
{
    switch (type) {
    case gearoenix_render_scene_type_index:
        return ICON_WORLD;
    case gearoenix_render_camera_type_index:
        return ICON_CAMERA;
    case gearoenix_render_light_directional_type_index:
    case gearoenix_render_light_shadow_caster_directional_type_index:
        return ICON_LIGHT_DIR;
    case gearoenix_render_light_point_type_index:
        return ICON_LIGHT_POINT;
    case gearoenix_render_light_type_index:
        return ICON_LIGHT;
    case gearoenix_render_model_type_index:
        return ICON_MODEL;
    case gearoenix_render_mesh_type_index:
        return ICON_MESH;
    case gearoenix_render_skybox_type_index:
        return ICON_SKYBOX;
    case gearoenix_render_material_type_index:
    case gearoenix_render_material_pbr_type_index:
    case gearoenix_render_material_sprite_type_index:
    case gearoenix_render_material_unlit_type_index:
        return ICON_MATERIAL;
    case gearoenix_physics_transformation_type_index:
        return ICON_TRANSFORM;
    case gearoenix_render_reflection_probe_type_index:
    case gearoenix_render_reflection_baked_type_index:
    case gearoenix_render_reflection_runtime_type_index:
        return ICON_REFLECTION;
    case gearoenix_physics_constraint_type_index:
    case gearoenix_physics_constraint_jet_controller_type_index:
        return ICON_CONSTRAINT;
    case gearoenix_physics_animation_bone_type_index:
    case gearoenix_physics_animation_armature_type_index:
    case gearoenix_physics_animation_player_type_index:
    case gearoenix_physics_animation_type_index:
    case gearoenix_physics_animation_armature_animation_type_index:
    case gearoenix_physics_animation_sprite_animation_type_index:
        return ICON_ANIMATION;
    case gearoenix_physics_collider_type_index:
    case gearoenix_physics_collider_aabb3_type_index:
    case gearoenix_physics_collider_frustum_type_index:
        return ICON_COLLIDER;
    default:
        return ICON_GENERIC;
    }
}

ImU32 gearoenix::editor::ui::icon_colour_for_component_type(const core::object_type_index_t type)
{
    // Light, low-saturation accents against the dark theme. Each type gets a distinct
    // hue so a column of icons reads like a legend.
    switch (type) {
    case gearoenix_render_scene_type_index:
        return IM_COL32(220, 196, 150, 255); // warm amber
    case gearoenix_render_camera_type_index:
        return IM_COL32(180, 206, 230, 255); // cool blue
    case gearoenix_render_light_directional_type_index:
    case gearoenix_render_light_shadow_caster_directional_type_index:
        return IM_COL32(230, 220, 170, 255); // pale gold
    case gearoenix_render_light_point_type_index:
        return IM_COL32(220, 200, 170, 255); // soft warm
    case gearoenix_render_light_type_index:
        return IM_COL32(220, 206, 160, 255);
    case gearoenix_render_model_type_index:
        return IM_COL32(190, 216, 180, 255); // sage
    case gearoenix_render_mesh_type_index:
        return IM_COL32(200, 200, 200, 255); // neutral
    case gearoenix_render_skybox_type_index:
        return IM_COL32(180, 210, 220, 255); // sky
    case gearoenix_render_material_type_index:
    case gearoenix_render_material_pbr_type_index:
    case gearoenix_render_material_sprite_type_index:
    case gearoenix_render_material_unlit_type_index:
        return IM_COL32(220, 196, 206, 255); // dusty pink
    case gearoenix_physics_transformation_type_index:
        return IM_COL32(206, 196, 222, 255); // lavender
    case gearoenix_render_reflection_probe_type_index:
    case gearoenix_render_reflection_baked_type_index:
    case gearoenix_render_reflection_runtime_type_index:
        return IM_COL32(216, 216, 224, 255); // silver
    case gearoenix_physics_constraint_type_index:
    case gearoenix_physics_constraint_jet_controller_type_index:
        return IM_COL32(180, 210, 206, 255); // muted teal
    case gearoenix_physics_animation_bone_type_index:
    case gearoenix_physics_animation_armature_type_index:
    case gearoenix_physics_animation_player_type_index:
    case gearoenix_physics_animation_type_index:
    case gearoenix_physics_animation_armature_animation_type_index:
    case gearoenix_physics_animation_sprite_animation_type_index:
        return IM_COL32(220, 190, 200, 255); // rose
    case gearoenix_physics_collider_type_index:
    case gearoenix_physics_collider_aabb3_type_index:
    case gearoenix_physics_collider_frustum_type_index:
        return IM_COL32(220, 180, 180, 255); // soft coral
    default:
        return IM_COL32(200, 200, 205, 255); // generic
    }
}

const char* gearoenix::editor::ui::icon_for_entity(const core::ecs::Entity* const entity)
{
    return icon_for_component_type(pick_primary_component_type(entity));
}

ImU32 gearoenix::editor::ui::icon_colour_for_entity(const core::ecs::Entity* const entity)
{
    return icon_colour_for_component_type(pick_primary_component_type(entity));
}

const char* gearoenix::editor::ui::short_component_type_name(const core::object_type_index_t type)
{
    // `get_object_type_name` returns a reference to a persistent string owned by the
    // object-type registry, so pointing into it is safe for the program's lifetime.
    // Returning `c_str() + offset` keeps the result null-terminated without allocating.
    const auto& full = core::Object::get_object_type_name(type);
    const auto pos = full.rfind("::");
    if (pos == std::string::npos) {
        return full.c_str();
    }
    return full.c_str() + pos + 2;
}

// =====================================================================================
//  Icon-aware widget helpers
// =====================================================================================

bool gearoenix::editor::ui::tree_node_with_icon(
    const char* const id,
    const ImGuiTreeNodeFlags flags,
    const char* const icon,
    const ImU32 icon_colour,
    const char* const display_name)
{
    build_padded_label(icon, display_name);
    const bool open = ImGui::TreeNodeEx(id, flags, "%s", g_label_scratch.c_str());
    // The icon sits at the start of the label content (after the arrow column),
    // which in ImGui equals `GetTreeNodeToLabelSpacing()` from the item's left.
    overdraw_icon_on_last_item(icon, icon_colour, ImGui::GetTreeNodeToLabelSpacing());
    return open;
}

bool gearoenix::editor::ui::tab_item_with_icon(
    const char* const id_suffix,
    const char* const icon,
    const ImU32 icon_colour,
    const char* const display_name)
{
    // The icon is baked directly into the tab's label — that way ImGui itself
    // renders and clips it alongside the name, so tabs that end up behind the
    // `<` / `>` scroll buttons can't paint their icon on top of the buttons.
    // `PushStyleColor(ImGuiCol_Text)` tints the entire tab label (icon + name)
    // with the component's accent colour; the overdraw/clipping dance the tree
    // nodes use isn't needed here.
    g_label_scratch.clear();
    g_label_scratch.append(icon);
    g_label_scratch.append(" ");
    g_label_scratch.append(display_name);
    g_label_scratch.append("##");
    if (nullptr != id_suffix) {
        g_label_scratch.append(id_suffix);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, icon_colour);
    const bool active = ImGui::BeginTabItem(g_label_scratch.c_str());
    ImGui::PopStyleColor();
    return active;
}
