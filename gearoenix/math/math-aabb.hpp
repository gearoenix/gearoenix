#ifndef GEAROENIX_MATH_AABB_HPP
#define GEAROENIX_MATH_AABB_HPP
#include "../core/cr-static.hpp"
#include "math-intersection-status.hpp"
#include "math-vector.hpp"
#include <optional>

namespace gearoenix::system::stream {
    class Stream;
}

namespace gearoenix::math {
struct Ray3;
struct Sphere;
struct Aabb3 {
    GX_GET_CREF_PRV(Vec3, upper)
    GX_GET_CREF_PRV(Vec3, lower)
    GX_GET_CREF_PRV(Vec3, diameter)
    GX_GET_CREF_PRV(Vec3, center)
    GX_GET_VAL_PRV(core::Real, volume, 0.0f)
public:
    Aabb3() noexcept;
    Aabb3(const Vec3& upper, const Vec3& lower) noexcept;
    explicit Aabb3(const Vec3& p) noexcept;
    void update() noexcept;
    void reset() noexcept;
    void reset(const Vec3& p) noexcept;
    void put(const Vec3& p) noexcept;
    void put(const Sphere& o) noexcept;
    void put(const Aabb3& o) noexcept;
    /// For better performance but keep in mind call update after your puts finished
    /// In case your not sure about it, use put, it is safer
    void put_without_update(const Vec3& p) noexcept;
    void put_without_update(const Aabb3& o) noexcept;
    std::optional<core::Real> hit(const math::Ray3& r, core::Real d_min) const noexcept;
    IntersectionStatus check_intersection_status(const Aabb3& o) const noexcept;
    bool check_intersection(const Sphere& o) const noexcept;
    bool check_intersection(const Aabb3& o, Aabb3& intersection) const noexcept;
    bool test(const Aabb3& o) const noexcept; // TODO it hase to change to check_intersection but be careful about previous check_intersection_status usages
    void set_center(const Vec3& c) noexcept;
    void set_diameter(const Vec3& d) noexcept;
    void read(system::stream::Stream* s) noexcept;
    void get_all_corners(math::Vec3 (&corners)[8]) noexcept;
};
}
#endif
