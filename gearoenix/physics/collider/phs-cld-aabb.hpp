#ifndef GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#define GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#include "phs-cld-collider.hpp"
#include "../../system/sys-log.hpp"
namespace gearoenix::physics::collider {
class AABB: public Collider {
public:
    AABB() noexcept = default;
    ~AABB() noexcept = default;
    std::optional<core::Real> hit(const math::Ray3&) const noexcept final { GXUNIMPLEMENTED }
};
}
#endif
