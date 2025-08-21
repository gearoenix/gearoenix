#pragma once
#include "../../math/gx-math-frustum.hpp"
#include "gx-phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
struct Frustum final : Collider {
    constexpr static auto max_count = 16;
    constexpr static auto object_type_index = gearoenix_physics_collider_frustum_type_index;
    constexpr static std::array all_parent_object_type_indices { Collider::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Collider::object_type_index };

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

    GX_GET_CREF_PRV(math::Frustum<double>, frustum);

    // void write_in_io_context(std::shared_ptr<platform::stream::Stream> &&, core::job::EndCaller<> &&) const override;
    // void update_in_io_context(std::shared_ptr<platform::stream::Stream> &&, core::job::EndCaller<> &&) override;
    void update_surrounding_box() override;

public:
    Frustum(core::ecs::Entity* entity, std::shared_ptr<Transformation>&& transform, std::string&& name, const std::array<math::Vec3<double>, 8>& points);
    /// For the arrangement of point go to the math::Frustum constructor
    void update(const std::array<math::Vec3<double>, 8>& points);
    [[nodiscard]] bool check_intersection(const math::Aabb3<double>& box) const override;
    [[nodiscard]] math::IntersectionStatus check_intersection_status(const math::Aabb3<double>& box) const override;
};
}