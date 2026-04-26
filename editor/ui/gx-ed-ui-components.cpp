#include "gx-ed-ui-components.hpp"
#include "gx-ed-ui-entities.hpp"
#include "gx-ed-ui-icons.hpp"
#include "gx-ed-ui-tooltip.hpp"

#include <gearoenix/core/ecs/gx-cr-ecs-component.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-entity.hpp>
#include <gearoenix/core/gx-cr-object.hpp>
#include <gearoenix/core/gx-cr-static-flat-set.hpp>

#include <ImGui/imgui.h>

#include <format>
#include <iterator>
#include <ranges>
#include <string>

namespace {
using UniqueComponentSet = gearoenix::core::static_flat_set<gearoenix::core::ecs::Component*, gearoenix::core::ecs::Entity::max_components>;

/// Collect every component attached to `entity` into a deduplicated set. The ECS stores
/// the same component pointer under its final-type key *and* each of its parent-type
/// keys, so a naïve iteration would visit one component multiple times.
[[nodiscard]] UniqueComponentSet collect_unique_components(
    const gearoenix::core::ecs::Entity& entity)
{
    UniqueComponentSet out;
    for (const auto& comp : entity.get_all_types_to_components() | std::views::values) {
        if (comp) {
            out.emplace(comp.get());
        }
    }
    return out;
}

/// Centred grey placeholder used when the panel has nothing to show.
void draw_centred_placeholder(const char* const msg)
{
    const auto avail = ImGui::GetContentRegionAvail();
    const auto text_sz = ImGui::CalcTextSize(msg);
    ImGui::SetCursorPos(ImVec2(
        ImGui::GetCursorPosX() + (avail.x - text_sz.x) * 0.5f,
        ImGui::GetCursorPosY() + (avail.y - text_sz.y) * 0.5f));
    ImGui::TextDisabled("%s", msg);
}

void draw_component_tabs(const gearoenix::core::ecs::Entity& entity)
{
    const auto unique_components = collect_unique_components(entity);
    if (unique_components.empty()) {
        ImGui::TextDisabled("This entity has no components.");
        return;
    }

    constexpr auto tab_bar_flags = ImGuiTabBarFlags_Reorderable
        | ImGuiTabBarFlags_TabListPopupButton
        | ImGuiTabBarFlags_FittingPolicyScroll;

    if (!ImGui::BeginTabBar("##components-tabs", tab_bar_flags)) {
        return;
    }

    static std::string id_suffix;

    for (auto* const component : unique_components) {
        const auto type = component->get_object_final_type_index();
        const char* const short_name = gearoenix::editor::ui::short_component_type_name(type);

        id_suffix.clear();
        std::format_to(std::back_inserter(id_suffix), "{}", component->get_object_id());

        const bool active = gearoenix::editor::ui::tab_item_with_icon(
            id_suffix.c_str(),
            gearoenix::editor::ui::icon_for_component_type(type),
            gearoenix::editor::ui::icon_colour_for_component_type(type),
            short_name);
        gearoenix::editor::ui::tooltip(short_name);

        if (active) {
            if (ImGui::BeginChild("##components-tab-body", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
                component->show_debug_gui();
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();
}
}

void gearoenix::editor::ui::Components::draw_header_inline_content(const float avail_w)
{
    (void)avail_w;
    const auto* const entity = Entities::get().get_selected();
    if (nullptr == entity) {
        return;
    }
    // Icon is pushed in the entity's accent colour; the name follows after a
    // font-size-proportional gap so it breathes at any UI scale.
    ImGui::PushStyleColor(ImGuiCol_Text, icon_colour_for_entity(entity));
    ImGui::TextUnformatted(icon_for_entity(entity));
    ImGui::PopStyleColor();
    ImGui::SameLine(0.0f, ImGui::GetFontSize() * 0.50f);
    ImGui::TextUnformatted(entity->get_object_name().c_str());
}

gearoenix::editor::ui::Components::Components()
    : Singleton(this)
    , DockableSection("Components")
{
}

gearoenix::editor::ui::Components::~Components() = default;

void gearoenix::editor::ui::Components::update()
{
    const auto* const entity = Entities::get().get_selected();
    if (nullptr == entity) {
        draw_centred_placeholder("No entity selected");
        return;
    }
    draw_component_tabs(*entity);
}