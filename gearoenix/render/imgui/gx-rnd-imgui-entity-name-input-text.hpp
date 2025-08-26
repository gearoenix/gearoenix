#pragma once
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-rnd-imgui-style-wrong-input-text.hpp"
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace gearoenix::render::imgui {
inline bool entity_name_text_input(std::string& name)
{
    const auto _ = set_wrong_input_text_style(core::ecs::World::get().get_entity(name).has_value());
    return ImGui::InputTextWithHint("Entity Name", "Must be a unique name", &name);
}
}