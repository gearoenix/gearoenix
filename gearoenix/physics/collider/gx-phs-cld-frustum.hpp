#ifndef GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#define GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-frustum.hpp"

namespace gearoenix::physics::collider {
struct Frustum final : public core::ecs::Component {
public:
    GX_GET_CREF_PRV(math::Aabb3<double>, surrounding_box);
    GX_GET_CREF_PRV(math::Frustum<double>, frustum);

public:
    explicit Frustum(
        std::string&& name,
        const std::array<math::Vec3<double>, 8>& points = {
            math::Vec3<double>(-1.0, 1.0, 1.0),
            math::Vec3<double>(1.0, 1.0, 1.0),
            math::Vec3<double>(-1.0, 1.0, -1.0),
            math::Vec3<double>(1.0, 1.0, -1.0),
            math::Vec3<double>(-1.0, -1.0, 1.0),
            math::Vec3<double>(1.0, -1.0, 1.0),
            math::Vec3<double>(-1.0, -1.0, -1.0),
            math::Vec3<double>(1.0, -1.0, -1.0) }) noexcept;
    Frustum(Frustum&&) noexcept;
    /// For the arrangement of point go to the math::Frustum constructor
    void update(const std::array<math::Vec3<double>, 8>& points) noexcept;
    [[nodiscard]] bool check_intersection(const math::Aabb3<double>& box) const noexcept;
    [[nodiscard]] math::IntersectionStatus check_intersection_status(const math::Aabb3<double>& box) const noexcept;
};
}
#endif