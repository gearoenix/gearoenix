#ifndef GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#define GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"

namespace gearoenix::physics::collider {
struct Aabb3 final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Aabb3<double>, original_box);
    GX_GET_CREF_PRV(math::Aabb3<double>, updated_box);

public:
    Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name);
    Aabb3(const math::Aabb3<double>& original_box, std::string&& name);
    [[nodiscard]] static std::shared_ptr<Aabb3> construct(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name);
    [[nodiscard]] static std::shared_ptr<Aabb3> construct(const math::Aabb3<double>& original_box, std::string&& name);
    ~Aabb3() final;
    void update(const math::Mat4x4<double>& transform);
    [[nodiscard]] const boost::container::flat_set<std::type_index>& get_all_the_hierarchy_types_except_component() const final;
};
}
#endif
