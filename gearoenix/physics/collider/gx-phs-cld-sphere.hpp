#ifndef GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#define GEAROENIX_PHYSICS_COLLIDER_SPHERE_HPP
#include "../../math/gx-math-sphere.hpp"
#include "gx-phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
struct Sphere final : public Collider {
    GX_GET_CREF_PRV(math::Sphere, sphere)
public:
    Sphere(const math::Vec3<double>& c, double r) noexcept;
    ~Sphere() noexcept final = default;
    void set_location(const math::Vec3<double>& l) noexcept final;
    void local_scale(double s) noexcept final;
    void local_x_scale(double s) noexcept final;
    [[nodiscard]] std::optional<double> hit(const math::Ray3& r, double d_min) const noexcept final;
};
}
#endif
