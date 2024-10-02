#ifndef GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#define GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"

namespace gearoenix::physics::collider {
struct Aabb3 final : core::ecs::Component {
    GX_GET_CREF_PRV(math::Aabb3<double>, original_box);
    GX_GET_CREF_PRV(math::Aabb3<double>, updated_box);

    [[nodiscard]] const HierarchyTypes& get_hierarchy_types() const override;

public:
    Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name, core::ecs::entity_id_t entity_id);
    Aabb3(const math::Aabb3<double>& original_box, std::string&& name, core::ecs::entity_id_t entity_id);
    [[nodiscard]] static std::shared_ptr<Aabb3> construct(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name, core::ecs::entity_id_t entity_id);
    [[nodiscard]] static std::shared_ptr<Aabb3> construct(const math::Aabb3<double>& original_box, std::string&& name, core::ecs::entity_id_t entity_id);
    ~Aabb3() override;
    void update(const math::Mat4x4<double>& transform);
};
}
#endif
