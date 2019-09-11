#ifndef GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#define GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#include "phs-cld-collider.hpp"
namespace gearoenix::physics::collider {
class Aabb: public Collider {
public:
    Aabb(math::Vec3 u, math::Vec3 l) noexcept;
    ~Aabb() noexcept = default;
};
}
#endif
