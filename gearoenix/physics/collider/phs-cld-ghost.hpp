#ifndef GEAROENIX_PHYSICS_COLLIDER_GHOST_HPP
#define GEAROENIX_PHYSICS_COLLIDER_GHOST_HPP
#include "phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
class Ghost final : public Collider {
public:
    Ghost() noexcept
        : Collider(Type::Ghost)
    {
    }
    ~Ghost() noexcept final = default;
    [[nodiscard]] std::optional<double> hit(const math::Ray3&, double) const noexcept final { return std::nullopt; }
};
}
#endif
