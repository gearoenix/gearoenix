#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"

namespace gearoenix::physics::collider {
struct Aabb3 final : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 8192;
    constexpr static TypeIndex TYPE_INDEX = 1;
    constexpr static TypeIndexSet ALL_PARENT_TYPE_INDICES {};
    constexpr static TypeIndexSet IMMEDIATE_PARENT_TYPE_INDICES {};

    GX_GET_CREF_PRV(math::Aabb3<double>, original_box);
    GX_GET_CREF_PRV(math::Aabb3<double>, updated_box);

public:
    Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name, core::ecs::entity_id_t entity_id);
    Aabb3(const math::Aabb3<double>& original_box, std::string&& name, core::ecs::entity_id_t entity_id);
    ~Aabb3() override;
    void update(const math::Mat4x4<double>& transform);
};
}