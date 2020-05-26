#ifndef GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#define GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#include "phs-cld-collider.hpp"
namespace gearoenix::physics::collider {
class Aabb final : public Collider {
public:
    Aabb(const math::Vec3<double>& u, const math::Vec3<double>& l) noexcept
        : Collider(Type::Aabb)
    {
        origin_box = math::Aabb3(u, l);
        updated_box = origin_box;
    }

    Aabb(const Collider& o) noexcept
        : Collider(Type::Aabb, o)
    {
    }

    ~Aabb() noexcept final = default;
};
}
#endif
