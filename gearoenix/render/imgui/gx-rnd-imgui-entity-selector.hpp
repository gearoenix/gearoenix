#ifndef GEAROENIX_RENDER_IMGUI_ENTITY_SELECTOR_HPP
#define GEAROENIX_RENDER_IMGUI_ENTITY_SELECTOR_HPP
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include <imgui/imgui.h>

namespace gearoenix::render::imgui {
struct EntitySelector final {
    engine::Engine& e;

    GX_GET_CREF_PRV(std::vector<const char*>, entity_names);
    GX_GET_CREF_PRV(std::vector<core::ecs::entity_id_t>, ids);
    std::uint64_t last_update = 0;
    int current_selection = -1;

public:
    explicit EntitySelector(engine::Engine& e)
        : e(e)
    {
    }

    EntitySelector(EntitySelector&&) = delete;
    EntitySelector(const EntitySelector&) = delete;

    template <typename Condition>
    void show()
    {
        if (e.get_frame_number_from_start() != last_update) {
            last_update = e.get_frame_number_from_start();

            entity_names.clear();
            ids.clear();

            e.get_world()->synchronised_system<Condition>([this](const auto id, const auto...) {
                ids.push_back(id);
                entity_names.push_back(e.get_world()->get_entity(id)->get_name().c_str());
            });
        }

        ImGui::Combo("Select Scene", &current_selection, entity_names.data(), static_cast<int>(entity_names.size()));
    }

    [[nodiscard]] bool selected() const
    {
        return current_selection >= 0;
    }

    [[nodiscard]] core::ecs::entity_id_t get_selection() const
    {
        if (selected()) {
            return ids[current_selection];
        }
        return core::ecs::INVALID_ENTITY_ID;
    }
};
}

#endif