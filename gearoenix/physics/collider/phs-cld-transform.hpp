#ifndef GEAROENIX_PHYSICS_COLLIDER_TRANSFORM_HPP
#define GEAROENIX_PHYSICS_COLLIDER_TRANSFORM_HPP
#include "phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
class Transform final : public Collider {
public:
    GX_GETSET_CREF_PRV(math::Mat4x4<double>, matrix)
    GX_GET_REF_PRV(math::Aabb3, limit)
public:
    explicit Transform() noexcept;
    ~Transform() noexcept final;

    [[nodiscard]] std::optional<double> hit(const math::Ray3& r, double d_min) const noexcept final;
    [[nodiscard]] bool check_intersection(const math::Aabb3& box) const noexcept final;
    [[nodiscard]] math::IntersectionStatus check_intersection_status(const math::Aabb3& box) const noexcept final;

    [[nodiscard]] math::Vec3<double> get_location() const noexcept final;
    void set_location(const math::Vec3<double>&) noexcept final;
    void local_scale(double) noexcept final;
    void local_x_scale(double) noexcept final;
};
}
#endif