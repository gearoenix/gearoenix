#ifndef GEAROENIX_PHYSICS_COLLIDER_GHOST_HPP
#define GEAROENIX_PHYSICS_COLLIDER_GHOST_HPP
#include "gx-phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
struct Ghost final : public Collider {
public:
    Ghost() noexcept
        : Collider(Type::Ghost)
    {
    }
    Ghost(const Ghost& o) noexcept = default;
    ~Ghost() noexcept final = default;
    [[nodiscard]] std::optional<double> hit(const math::Ray3&, double) const noexcept final { return std::nullopt; }

    [[nodiscard]] Collider* clone() const noexcept final
    {
        return new Ghost(*this);
    }
};
}
#endif
