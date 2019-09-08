#ifndef GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#define GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#include "phs-cld-collider.hpp"
#include "../../system/sys-log.hpp"
namespace gearoenix::physics::collider {
class Aabb: public Collider {
public:
    Aabb(math::Vec3 u, math::Vec3 l) noexcept;
    ~Aabb() noexcept = default;
    // std::optional<core::Real> hit(const math::Ray3&, core::Real) const noexcept final;
};
}
#endif
