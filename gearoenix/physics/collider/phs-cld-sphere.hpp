#ifndef GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#define GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#include "../../math/math-sphere.hpp"
#include "phs-cld-collider.hpp"
namespace gearoenix::physics::collider {
class Sphere : public Collider {
    GX_GET_CREF_PRV(math::Sphere, sphere)
public:
    Sphere(const math::Vec3& c, core::Real r) noexcept;
    ~Sphere() noexcept final = default;
    void set_location(const math::Vec3& l) noexcept final;
    void local_scale(core::Real s) noexcept final;
    void local_x_scale(core::Real s) noexcept final;
    [[nodiscard]] std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept final;
};
}
#endif
