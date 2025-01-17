#include "gx-rnd-lt-light.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"

gearoenix::render::light::Light::Light(
    const core::ecs::component_index_t final_type_index,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(final_type_index, std::move(name), entity_id)
{
}

gearoenix::render::light::Light::~Light() = default;

void gearoenix::render::light::Light::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        Component::show_debug_gui();
        ImGui::InputFloat3("Colour", reinterpret_cast<float*>(&colour));
    });
}
