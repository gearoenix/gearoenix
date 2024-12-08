#pragma once
#include "gx-rnd-lt-light.hpp"

namespace gearoenix::render::light {
struct Point final : Light {
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 14;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES { Light::TYPE_INDEX };
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES { Light::TYPE_INDEX };

    math::Vec3<float> position = { 0.0f, 0.0f, 0.0f };

    Point(std::string&& name, core::ecs::entity_id_t entity_id);
    ~Point() override;
};
}