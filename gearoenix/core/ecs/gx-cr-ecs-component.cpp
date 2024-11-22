#include "gx-cr-ecs-component.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-cr-string.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../macro/gx-cr-mcr-stringifier.hpp"
#include "gx-cr-ecs-world.hpp"
#include <imgui/imgui.h>

boost::container::flat_map<std::type_index, std::string> gearoenix::core::ecs::Component::type_index_to_name;

boost::container::flat_map<std::string, std::type_index> gearoenix::core::ecs::Component::type_name_to_index;

void gearoenix::core::ecs::Component::register_type(std::type_index t, std::string name)
{
#if GX_DEBUG_MODE
    if (const auto search = type_index_to_name.find(t); type_index_to_name.end() != search) {
        GX_ASSERT_D(search->second == name);
    }
    if (const auto search = type_name_to_index.find(name); type_name_to_index.end() != search) {
        GX_ASSERT_D(search->second == t);
    }
#endif
    type_index_to_name.emplace(t, name);
    type_name_to_index.emplace(std::move(name), t);
}

gearoenix::core::ecs::Component::Component(const std::type_index final_type_index, std::string&& name, const entity_id_t entity_id)
    : final_type_index(final_type_index)
    , name(std::move(name))
    , entity_id(entity_id)
{
}

void gearoenix::core::ecs::Component::set_component_self(const std::shared_ptr<Component>& c)
{
    component_self = c;
}

void gearoenix::core::ecs::Component::show_debug_gui(const render::engine::Engine& e)
{
    if (!ImGui::TreeNode(String::ptr_name(this).c_str())) {
        return;
    }

    if (!ImGui::BeginTable("##gearoenix::core::ecs::Component", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {
        return;
    }
    ImGui::TableSetupColumn("##labels", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("##inputs", ImGuiTableColumnFlags_WidthStretch, 0.999f);

    ImGui::TableNextColumn();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(enabled), &enabled);
    ImGui::TableNextColumn();

    ImGui::Text("Name:");
    ImGui::TableNextColumn();
    ImGui::Text("%s", name.c_str());
    ImGui::TableNextColumn();

    const auto type_search = type_index_to_name.find(final_type_index);
    ImGui::Text("Final Type: ");
    ImGui::TableNextColumn();
    ImGui::Text("%s", type_search != type_index_to_name.end() ? type_search->second.c_str() : "The type is not registered!");
    ImGui::TableNextColumn();

    ImGui::Text("Entity ID:");
    ImGui::TableNextColumn();
    ImGui::Text("%u", entity_id);

    ImGui::EndTable();

    if (const auto* const entity_ptr = e.get_world()->get_entity(entity_id); nullptr != entity_ptr && ImGui::TreeNode("Entity")) {
        entity_ptr->show_debug_gui(e);
        ImGui::TreePop();
    }

    ImGui::TreePop();
}
