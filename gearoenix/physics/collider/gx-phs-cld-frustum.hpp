#ifndef GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#define GEAROENIX_PHYSICS_COLLIDER_FRUSTUM_HPP
#include "../../math/gx-math-frustum.hpp"
#include "gx-phs-cld-collider.hpp"

namespace gearoenix::physics::collider {
struct Frustum final : public Collider {
public:
    GX_GET_REF_PRV(math::Frustum<>, frustum)
public:
    explicit Frustum(const math::Vec3<double> (&points)[8] = {
                         math::Vec3(-1.0, 1.0, 1.0),
                         math::Vec3(1.0, 1.0, 1.0),
                         math::Vec3(-1.0, 1.0, -1.0),
                         math::Vec3(1.0, 1.0, -1.0),
                         math::Vec3(-1.0, -1.0, 1.0),
                         math::Vec3(1.0, -1.0, 1.0),
                         math::Vec3(-1.0, -1.0, -1.0),
                         math::Vec3(1.0, -1.0, -1.0),
                     }) noexcept;
    ~Frustum() noexcept final;

    /// For the arrangement of point go to the math::Frustum constructor
    void update(const math::Vec3<double> (&points)[8]) noexcept;

    [[nodiscard]] std::optional<double> hit(const math::Ray3& r, double d_min) const noexcept final;
    [[nodiscard]] bool check_intersection(const math::Aabb3& box) const noexcept final;
    [[nodiscard]] math::IntersectionStatus check_intersection_status(const math::Aabb3& box) const noexcept final;

    [[nodiscard]] math::Vec3<double> get_location() const noexcept final;
    void set_location(const math::Vec3<double>&) noexcept final;
    void local_scale(double) noexcept final;
    void local_x_scale(double) noexcept final;
};
}
#endif