#include "gx-rnd-scn-selector.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-scn-scene.hpp"
#include <imgui/imgui.h>

gearoenix::render::scene::Selector::Selector(engine::Engine& e)
    : e(e)
{
}

gearoenix::render::scene::Selector::~Selector() = default;

void gearoenix::render::scene::Selector::show()
{
    if (e.get_frame_number_from_start() != last_update) {
        last_update = e.get_frame_number_from_start();

        entity_names.clear();
        ids.clear();

        entity_names.emplace_back("<No Scene>");
        ids.push_back(core::ecs::INVALID_ENTITY_ID);

        e.get_world()->synchronised_system<Scene>([this](const auto id, const auto) {
            ids.push_back(id);
            entity_names.push_back(e.get_world()->get_entity(id)->get_name().c_str());
        });
    }

    ImGui::Combo("Select Scene", &current_selection, entity_names.data(), static_cast<int>(entity_names.size()));
}

gearoenix::core::ecs::entity_id_t gearoenix::render::scene::Selector::get_selection() const
{
    if (current_selection > 0) {
        return ids[current_selection];
    }
    return core::ecs::INVALID_ENTITY_ID;
}
