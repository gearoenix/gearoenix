#ifndef GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#define GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#include "../../math/math-sphere.hpp"
#include "phs-cld-collider.hpp"
namespace gearoenix::physics::collider {
class Sphere : public Collider {
private:
    math::Sphere sphere;
public:
    Sphere(const math::Vec3 &c, core::Real r) noexcept;
    ~Sphere() noexcept = default;
    std::optional<core::Real> hit(const math::Ray3& r) const noexcept final;
};
}
#endif
