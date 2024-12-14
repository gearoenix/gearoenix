#pragma once
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-frustum.hpp"

namespace gearoenix::physics::collider {
struct Frustum final : core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 16;
    constexpr static core::ecs::component_index_t TYPE_INDEX = 3;
    constexpr static core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    constexpr static auto default_points = std::array {
        math::Vec3(-1.0, 1.0, 1.0),
        math::Vec3(1.0, 1.0, 1.0),
        math::Vec3(-1.0, 1.0, -1.0),
        math::Vec3(1.0, 1.0, -1.0),
        math::Vec3(-1.0, -1.0, 1.0),
        math::Vec3(1.0, -1.0, 1.0),
        math::Vec3(-1.0, -1.0, -1.0),
        math::Vec3(1.0, -1.0, -1.0)
    };

    GX_GET_CREF_PRV(math::Aabb3<double>, surrounding_box);
    GX_GET_CREF_PRV(math::Frustum<double>, frustum);

    void write_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) const override;
    void update_in_io_context(std::shared_ptr<platform::stream::Stream>&&, core::job::EndCaller<>&&) override;

public:
    Frustum(std::string&& name, const std::array<math::Vec3<double>, 8>& points, core::ecs::entity_id_t entity_id);
    /// For the arrangement of point go to the math::Frustum constructor
    void update(const std::array<math::Vec3<double>, 8>& points);
    [[nodiscard]] bool check_intersection(const math::Aabb3<double>& box) const;
    [[nodiscard]] math::IntersectionStatus check_intersection_status(const math::Aabb3<double>& box) const;
};
}