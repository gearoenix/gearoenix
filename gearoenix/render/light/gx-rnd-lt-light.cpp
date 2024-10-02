#include "gx-rnd-lt-light.hpp"
#include <imgui/imgui.h>

gearoenix::render::light::Light::Light(
    const std::type_index final_type_index,
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Component(final_type_index, std::move(name), entity_id)
{
}

gearoenix::render::light::Light::~Light() = default;

void gearoenix::render::light::Light::show_debug_gui()
{
    if (ImGui::TreeNode("Light")) {
        ImGui::InputFloat3("Colour", reinterpret_cast<float*>(&colour));
        ImGui::TreePop();
    }
}
