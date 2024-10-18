#ifndef GEAROENIX_RENDER_IMGUI_ENTITY_NAME_TEXT_INPUT_HPP
#define GEAROENIX_RENDER_IMGUI_ENTITY_NAME_TEXT_INPUT_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "gx-rnd-imgui-style-wrong-input-text.hpp"
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace gearoenix::render::imgui {
inline bool entity_name_text_input(core::ecs::World& world, std::string& name)
{
    const auto _ = set_wrong_input_text_style(world.get_entity(name) == nullptr);
    return ImGui::InputTextWithHint("Entity Name", "Must be a unique name", &name);
}
}

#endif