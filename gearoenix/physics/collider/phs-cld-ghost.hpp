#ifndef GEAROENIX_PHYSICS_COLLIDER_GHOST_HPP
#define GEAROENIX_PHYSICS_COLLIDER_GHOST_HPP
#include "phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
class Ghost : public Collider {
public:
    Ghost() noexcept
        : Collider(Type::Ghost)
    {
    }
    ~Ghost() noexcept final = default;
    [[nodiscard]] std::optional<core::Real> hit(const math::Ray3&, const core::Real) const noexcept final { return std::nullopt; }
};
}
#endif
