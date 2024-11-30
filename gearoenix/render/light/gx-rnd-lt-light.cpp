#include "gx-rnd-lt-light.hpp"
#include "../../core/gx-cr-string.hpp"
#include <imgui/imgui.h>

gearoenix::render::light::Light::Light(
    const TypeIndex final_type_index,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(final_type_index, std::move(name), entity_id)
{
}

gearoenix::render::light::Light::~Light() = default;

void gearoenix::render::light::Light::show_debug_gui(const engine::Engine& e)
{
    if (!ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        return;
    }
    Component::show_debug_gui(e);
    ImGui::InputFloat3("Colour", reinterpret_cast<float*>(&colour));
    ImGui::TreePop();
}
