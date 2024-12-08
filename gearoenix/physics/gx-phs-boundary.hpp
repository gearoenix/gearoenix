#pragma once
#include "../core/ecs/gx-cr-ecs-component.hpp"
#include "../math/gx-math-aabb.hpp"

namespace gearoenix::physics {
struct Boundary final : core::ecs::Component {
    constexpr static core::ecs::component_index_t TYPE_INDEX = 4;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_CREF_PRV(math::Aabb3<double>, box);

    Boundary(
        const math::Vec3<double>& upper,
        const math::Vec3<double>& lower,
        std::string&& name,
        core::ecs::entity_id_t entity_id);

public:
    Boundary(const Boundary&) = delete;
    Boundary& operator=(Boundary&&) = delete;
    Boundary& operator=(const Boundary&) = delete;
};
}