#pragma once
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"

#include <ImGui/imgui.h>

namespace gearoenix::render::imgui {
struct EntitySelector final {
    GX_GET_CREF_PRV(std::vector<const char*>, entity_names);
    GX_GET_CREF_PRV(std::vector<core::ecs::Entity*>, ids);
    std::uint64_t last_update = 0;
    int current_selection = -1;

public:
    EntitySelector() = default;

    EntitySelector(EntitySelector&&) = delete;
    EntitySelector(const EntitySelector&) = delete;

    template <typename Condition>
    void show()
    {
        if (engine::Engine::get().get_frame_number_from_start() != last_update) {
            last_update = engine::Engine::get().get_frame_number_from_start();

            entity_names.clear();
            ids.clear();

            core::ecs::World::get().synchronised_system<Condition>([this](auto* const e, const auto...) {
                ids.push_back(e);
                entity_names.push_back(e->get_object_name().c_str());
            });
        }

        if (entity_names.empty()) {
            current_selection = -1;
        } else {
            ImGui::Combo("Select Scene", &current_selection, entity_names.data(), static_cast<int>(entity_names.size()));
        }
    }

    [[nodiscard]] bool selected() const
    {
        return current_selection >= 0;
    }

    [[nodiscard]] core::ecs::Entity* get_selection() const
    {
        if (selected()) {
            return ids[current_selection];
        }
        return nullptr;
    }
};
}