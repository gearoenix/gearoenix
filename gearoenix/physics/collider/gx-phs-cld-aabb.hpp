#ifndef GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#define GEAROENIX_PHYSICS_COLLIDER_AABB_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"

namespace gearoenix::physics::collider {
struct Aabb3 final : public core::ecs::Component {
    GX_GET_CREF_PRV(math::Aabb3<double>, original_box);
    GX_GET_CREF_PRV(math::Aabb3<double>, updated_box);

public:
    Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower) noexcept;
    explicit Aabb3(const math::Aabb3<double>& original_box) noexcept;
    Aabb3(Aabb3&&) noexcept;
    ~Aabb3() noexcept final;
    void update(const math::Mat4x4<double>& transform) noexcept;
};
}
#endif
