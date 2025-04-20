#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <string>

namespace gearoenix::render::light {
struct Builder;
struct Manager {
    Manager();
    virtual ~Manager();
    [[nodiscard]] static Manager& get();

    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;

    [[nodiscard]] virtual core::ecs::EntityPtr build_directional(std::string&& name, core::ecs::Entity* parent);
    virtual void build_shadow_caster_directional(
        std::string&& name,
        core::ecs::Entity* parent,
        std::uint32_t shadow_map_resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback);
};
}