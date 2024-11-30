#include "gx-cr-ecs-component.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-cr-string.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../macro/gx-cr-mcr-stringifier.hpp"
#include "gx-cr-ecs-world.hpp"
#include <imgui/imgui.h>

gearoenix::core::ecs::Component::TypesInfos gearoenix::core::ecs::Component::types_infos {};

boost::container::flat_map<std::string, gearoenix::core::ecs::Component::TypeIndex> gearoenix::core::ecs::Component::type_name_to_index;

void gearoenix::core::ecs::Component::register_type(TypeIndex t, TypeInfo&& info)
{
    GX_ASSERT_D(types_infos.size() > t);

    auto& ti = types_infos[t];
    if (ti.get_name() != info.get_name()) {
        GX_ASSERT_D(ti.get_name().empty());
        GX_ASSERT_D(type_name_to_index.find(info.get_name()) == type_name_to_index.end());

        type_name_to_index.emplace(info.get_name(), t);
    }

    ti = std::move(info);
}

gearoenix::core::ecs::Component::Component(const TypeIndex final_type_index, std::string&& name, const entity_id_t entity_id)
    : final_type_index(final_type_index)
    , name(std::move(name))
    , entity_id(entity_id)
{
    GX_ASSERT_D(final_type_index < types_infos.size());
    GX_ASSERT_D(!types_infos[final_type_index].get_name().empty());
    GX_ASSERT_D(type_name_to_index.find(types_infos[final_type_index].get_name()) != type_name_to_index.end());
    GX_ASSERT_D(type_name_to_index[types_infos[final_type_index].get_name()] == final_type_index);
#if GX_DEBUG_MODE
    for (const auto pt : types_infos[final_type_index].get_all_parents()) {
        GX_ASSERT_D(!types_infos[pt].get_name().empty());
        GX_ASSERT_D(type_name_to_index.find(types_infos[pt].get_name()) != type_name_to_index.end());
        GX_ASSERT_D(type_name_to_index[types_infos[pt].get_name()] == pt);
    }
#endif
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

    const auto& type_info = types_infos[final_type_index];
    ImGui::Text("Final Type: ");
    ImGui::TableNextColumn();
    ImGui::Text("%s", type_info.get_name().c_str());
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

std::optional<gearoenix::core::ecs::Component::TypeIndex> gearoenix::core::ecs::Component::get_type_index(const std::string& type_name)
{
    const auto search = type_name_to_index.find(type_name);
    return search == type_name_to_index.end() ? std::nullopt : std::make_optional(search->second);
}
