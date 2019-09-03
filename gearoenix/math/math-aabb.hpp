#ifndef GEAROENIX_MATH_AABB_HPP
#define GEAROENIX_MATH_AABB_HPP
#include "math-intersection-status.hpp"
#include "math-vector.hpp"
#include "../core/cr-static.hpp"
#include <optional>
namespace gearoenix::math {
struct Ray3;
struct Sphere;
struct Aabb3 {
    GX_GET_REF_PRV(Vec3, upper)
    GX_GET_REF_PRV(Vec3, lower)
    GX_GET_REF_PRV(Vec3, diameter)
    GX_GET_REF_PRV(Vec3, center)
    GX_GET_VAL_PRV(core::Real, volume, 0.0f)
public:
    /// TODO add without updates
    explicit Aabb3() noexcept;
    explicit Aabb3(const Vec3& upper, const Vec3& lower) noexcept;
    void update() noexcept;
    void reset() noexcept;
    void reset(const Vec3& p) noexcept;
    void put(const Vec3& p) noexcept;
    void put(const Sphere& o) noexcept;
    void put(const Aabb3& o) noexcept;
    /// For better performance but keep in mind call update after your puts finished
    /// In case your not sure about it, use put it is safer
    void put_without_update(const Aabb3& o) noexcept;
    bool test(const Aabb3& o, Aabb3& intersection) const noexcept;
    bool test(const Aabb3& o) const noexcept;
    bool test(const Sphere& o) const noexcept;
    std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept;
    IntersectionStatus::Type check_intersection(const Aabb3& o) const noexcept;
};
}
#endif
