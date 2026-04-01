#pragma once
#include "../../math/gx-math-sphere.hpp"

namespace gearoenix::physics::collider {
struct Sphere final {
    GX_GET_CREF_PRV(math::Sphere<core::fp_t>, sphere);

public:
    Sphere(const math::Vec3<core::fp_t>& c, const core::fp_t r)
        : sphere(c, r)
    {
    }
    void set_location(const math::Vec3<core::fp_t>& l) { sphere.set_center(l); }
    void local_scale(const core::fp_t s) { sphere.set_radius(sphere.get_radius() * s); }
    [[nodiscard]] std::optional<core::fp_t> hit(const math::Ray3<core::fp_t>& r, const core::fp_t d_min) const { return sphere.hit(r, d_min); }
};
}