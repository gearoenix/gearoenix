#ifndef GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#define GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-frustum.hpp"

namespace gearoenix::physics::collider {
struct Frustum final : public core::ecs::Component {
public:
    GX_GET_CREF_PRV(math::Aabb3<double>, surrounding_box);
    GX_GET_CREF_PRV(math::Frustum<double>, frustum);

private:
    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const final;

public:
    Frustum(std::string&& name, const std::array<math::Vec3<double>, 8>& points);
    [[nodiscard]] static std::shared_ptr<Frustum> construct(
        std::string&& name,
        const std::array<math::Vec3<double>, 8>& points = std::array<math::Vec3<double>, 8> {
            math::Vec3<double>(-1.0, 1.0, 1.0),
            math::Vec3<double>(1.0, 1.0, 1.0),
            math::Vec3<double>(-1.0, 1.0, -1.0),
            math::Vec3<double>(1.0, 1.0, -1.0),
            math::Vec3<double>(-1.0, -1.0, 1.0),
            math::Vec3<double>(1.0, -1.0, 1.0),
            math::Vec3<double>(-1.0, -1.0, -1.0),
            math::Vec3<double>(1.0, -1.0, -1.0) });
    /// For the arrangement of point go to the math::Frustum constructor
    void update(const std::array<math::Vec3<double>, 8>& points);
    [[nodiscard]] bool check_intersection(const math::Aabb3<double>& box) const;
    [[nodiscard]] math::IntersectionStatus check_intersection_status(const math::Aabb3<double>& box) const;
};
}
#endif