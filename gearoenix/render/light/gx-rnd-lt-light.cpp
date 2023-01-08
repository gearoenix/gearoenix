#include "gx-rnd-lt-light.hpp"
#include <imgui/imgui.h>

gearoenix::render::light::Light::Light(std::string&& name) noexcept
    : core::ecs::Component(this, std::move(name))
{
}

gearoenix::render::light::Light::Light(Light&&) noexcept = default;

gearoenix::render::light::Light::~Light() noexcept = default;

void gearoenix::render::light::Light::show_gui() noexcept
{
    if (ImGui::TreeNode("Light")) {
        ImGui::InputFloat3("Colour", reinterpret_cast<float*>(&colour));
        ImGui::TreePop();
    }
}