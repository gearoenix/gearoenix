#ifndef GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#define GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#include "phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
class Frustum : public Collider {
public:
    GX_GETSET_CREF_PRV(math::Mat4x4, view_projection)
    GX_GET_REF_PRV(math::Aabb3, limit)
public:
    Frustum() noexcept;
    ~Frustum() noexcept final;

    [[nodiscard]] std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept final;
    [[nodiscard]] bool check_intersection(const math::Aabb3& box) const noexcept final;
    [[nodiscard]] math::IntersectionStatus check_intersection_status(const math::Aabb3& box) const noexcept final;

    [[nodiscard]] const math::Vec3& get_location() const noexcept final;
    void set_location(const math::Vec3&) noexcept final;
    void local_scale(core::Real) noexcept final;
    void local_x_scale(core::Real) noexcept final;
};
}
#endif
