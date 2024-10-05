#include "gx-cr-ecs-component.hpp"
#include "../gx-cr-string.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../macro/gx-cr-mcr-stringifier.hpp"
#include <imgui/imgui.h>

#include "gx-cr-ecs-world.hpp"

namespace {
const auto type_name = boost::core::demangle(typeid(gearoenix::core::ecs::Component).name());
}

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

void gearoenix::core::ecs::Component::show_debug_gui(const World& w)
{
    const auto this_hex = String::ptr_to_hex_string(this);
    static std::string tree_id;
    tree_id.clear();
    tree_id += type_name;
    tree_id += " [";
    tree_id += this_hex.data();
    tree_id += "]";
    if (ImGui::TreeNode(tree_id.c_str())) {
        ImGui::Checkbox(GX_STRINGIFY_VAR(enabled), &enabled);
        ImGui::Text("%s: %s", GX_STRINGIFY_VAR(name), name.c_str());
        const auto type_search = type_index_to_name.find(final_type_index);
        ImGui::Text("%s: %s", GX_STRINGIFY_VAR(final_type_index), type_search != type_index_to_name.end() ? type_search->second.c_str() : "Is Not Registered.");
        const auto* const entity_ptr = w.get_entity(entity_id);
        ImGui::Text("%s: %u", GX_STRINGIFY_VAR(entity_id), entity_id);
        static std::string entity_node_id;
        entity_node_id.clear();
        entity_node_id += "Entity [";
        entity_node_id += this_hex.data();
        entity_node_id += "]";
        if (nullptr != entity_ptr && ImGui::TreeNode(entity_node_id.c_str())) {
            entity_ptr->show_debug_gui(w);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}
