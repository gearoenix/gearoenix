#include "gx-ed-ui-entities.hpp"
#include "gx-ed-ui-icons.hpp"
#include "gx-ed-ui-menu-scene.hpp"
#include "gx-ed-ui-tooltip.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-entity.hpp>
#include <gearoenix/core/gx-cr-object.hpp>
#include <gearoenix/render/imgui/gx-rnd-imgui-id.hpp>

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>

namespace {
/// Case-insensitive substring match — used to filter entities by the search box.
bool contains_case_insensitive(const std::string& haystack, const std::string& needle)
{
    if (needle.empty()) {
        return true;
    }
    if (haystack.size() < needle.size()) {
        return false;
    }
    const auto to_lower = [](const unsigned char c) { return std::tolower(c); };
    const auto it = std::ranges::search(
        haystack, needle,
        [&](const char a, const char b) {
            return to_lower(a) == to_lower(b);
        }).begin();
    return it != haystack.end();
}
}

gearoenix::editor::ui::Entities::Entities()
    : Singleton(this)
    , DockableSection("Entities")
{
}

gearoenix::editor::ui::Entities::~Entities() = default;

void gearoenix::editor::ui::Entities::set_selected(core::ecs::Entity* const entity)
{
    if (nullptr == entity) {
        selected.reset();
        return;
    }
    // Selection is a weak reference, so deleting the entity elsewhere automatically
    // clears it without this panel holding the last shared ref.
    selected = std::static_pointer_cast<core::ecs::Entity>(entity->get_object_self().lock());
}

gearoenix::core::ecs::Entity* gearoenix::editor::ui::Entities::get_selected() const
{
    return selected.lock().get();
}

bool gearoenix::editor::ui::Entities::matches_filter(const core::ecs::Entity* const entity) const
{
    if (search_filter.empty()) {
        return true;
    }
    if (contains_case_insensitive(entity->get_object_name(), search_filter)) {
        return true;
    }
    // A parent matches the filter if any of its descendants do — keeps the filter
    // hierarchical rather than hiding whole branches whose root happens not to match.
    return std::ranges::any_of(entity->get_children() | std::views::values, [this](const auto& child) {
        return matches_filter(child.get());
    });
}

void gearoenix::editor::ui::Entities::draw_entity_row(const core::ecs::Entity* const entity, const int depth)
{
    if (!matches_filter(entity)) {
        return;
    }

    const bool has_children = !entity->get_children().empty();
    const bool is_selected = selected.lock().get() == entity;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_OpenOnDoubleClick
        | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (is_selected) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (!has_children) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    if (depth == 0) {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    const auto id = render::imgui::create_id_string(entity);
    const bool open = tree_node_with_icon(
        id.data(), flags,
        icon_for_entity(entity), icon_colour_for_entity(entity),
        entity->get_object_name().c_str());

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        // Selection is the user's intent, not the ECS hierarchy's; const_cast is
        // justified because the ECS keeps entities mutable and we only pin a
        // weak_ptr — we never mutate `entity` through this path.
        set_selected(const_cast<core::ecs::Entity*>(entity));
    }

    tooltip(entity->get_object_name().c_str());

    if (!entity->get_enabled()) {
        ImGui::SameLine();
        ImGui::TextDisabled("(%s)", ICON_HIDDEN);
    }

    if (open) {
        for (const auto& child : entity->get_children() | std::views::values) {
            draw_entity_row(child.get(), depth + 1);
        }
        ImGui::TreePop();
    }
}

void gearoenix::editor::ui::Entities::draw_header_inline_content(const float avail_w)
{
    // `InputTextWithHint` overload from `imgui_stdlib.h` — ImGui manages growth of the
    // underlying `std::string` via a resize callback. No fixed buffer, no truncation,
    // and the string's capacity is retained across frames so steady state is zero-alloc.
    ImGui::SetNextItemWidth(avail_w);
    ImGui::InputTextWithHint("##entities-search", "filter by name...", &search_filter);
    tooltip("Filter entities by name");

    // Magnifying-glass icon overlaid at the right edge of the input frame so the field
    // reads as a search box at a glance. Drawn in the "disabled" text colour, so it's a
    // subtle indicator rather than competing with the user's text.
    //
    // To prevent long typed text from scrolling under the glyph, we first paint a
    // mask rectangle in the input's current frame-bg colour (matching hover / active
    // state) right where the icon sits — the text gets a clean visual stop at the
    // mask's left edge. The icon itself is then drawn with a `cpu_fine_clip_rect` that
    // is the intersection of the input's rect and the draw list's current clip, so
    // the icon cannot bleed outside the input when the input itself is narrow, and it
    // gets clipped along with the rest of the panel when the panel is too small.
    const auto& style = ImGui::GetStyle();
    const ImVec2 rect_min = ImGui::GetItemRectMin();
    const ImVec2 rect_max = ImGui::GetItemRectMax();
    const ImVec2 icon_sz = ImGui::CalcTextSize(ICON_SEARCH);
    const float icon_pad = style.FramePadding.x;
    const float icon_x = rect_max.x - icon_sz.x - icon_pad;
    const float icon_y = rect_min.y + style.FramePadding.y;

    // If the input is too narrow for the icon to fit inside its left edge, give up —
    // there's no sensible place for it and masking would cover the entire input.
    if (icon_x < rect_min.x + icon_pad) {
        return;
    }

    auto* const dl = ImGui::GetWindowDrawList();

    // Match the input's current background: ImGui swaps between FrameBg / FrameBgHovered
    // / FrameBgActive based on state, so reading those colours here keeps the mask
    // invisible regardless of whether the user is hovering or editing.
    const ImU32 mask_col = ImGui::GetColorU32(
        ImGui::IsItemActive()        ? ImGuiCol_FrameBgActive
            : ImGui::IsItemHovered() ? ImGuiCol_FrameBgHovered
                                     : ImGuiCol_FrameBg);
    // Leave 1 px inset from the input's outer rect so the frame's border/rounding stays
    // visible. `RoundCornersRight` keeps the rounding consistent with the input's own
    // right corners for themes with non-zero FrameRounding.
    const ImVec2 mask_min { icon_x - icon_pad, rect_min.y + 1.0f };
    const ImVec2 mask_max { rect_max.x - 1.0f, rect_max.y - 1.0f };
    dl->AddRectFilled(mask_min, mask_max, mask_col, style.FrameRounding, ImDrawFlags_RoundCornersRight);

    // Clip the icon to the visible intersection of the input's rect and the draw
    // list's current clip — covers both "input too narrow" and "panel clipped".
    const ImVec2 clip_min = dl->GetClipRectMin();
    const ImVec2 clip_max = dl->GetClipRectMax();
    const ImVec4 clip_rect {
        std::max(rect_min.x, clip_min.x),
        std::max(rect_min.y, clip_min.y),
        std::min(rect_max.x, clip_max.x),
        std::min(rect_max.y, clip_max.y),
    };
    dl->AddText(nullptr, 0.0f, { icon_x, icon_y },
        ImGui::GetColorU32(ImGuiCol_TextDisabled), ICON_SEARCH, nullptr, 0.0f, &clip_rect);
}

void gearoenix::editor::ui::Entities::update()
{
    if (!ImGui::BeginChild("##entities-tree", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::EndChild();
        return;
    }

    if (const auto& scenes = MenuScene::get(); !scenes.has_active_scene()) {
        ImGui::TextDisabled("No active scenes.");
        ImGui::TextDisabled("Create one from the Scene menu.");
    } else {
        for (const auto& e : scenes.get_active_scenes()) {
            draw_entity_row(e.get(), 0);
        }
    }

    ImGui::EndChild();
}