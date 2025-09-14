#include "gx-rnd-imgui-entity-name-input-text.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-rnd-imgui-style-wrong-input-text.hpp"

#include <ImGui/misc/cpp/imgui_stdlib.h>

std::pair<bool, bool> gearoenix::render::imgui::entity_name_text_input(std::string& name, const std::optional<float> width)
{
    const auto w = set_wrong_input_text_style(!name.empty() && !core::ecs::World::get().get_entity(name).has_value());
    if (width.has_value()) {
        ImGui::SetNextItemWidth(*width);
    }
    const auto input_changed = ImGui::InputTextWithHint("##gearoenix::render::imgui::entity_name_text_input", "Must be a unique name", &name);
    return { input_changed, w.is_valid };
}