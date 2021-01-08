#ifndef GEAROENIX_MATH_AABB_HPP
#define GEAROENIX_MATH_AABB_HPP
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-math-intersection-status.hpp"
#include "gx-math-vector-3d.hpp"
#include <optional>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::math {
struct Ray3;
struct Sphere;
struct Aabb3 {
    GX_GET_CREF_PRV(Vec3<double>, upper)
    GX_GET_CREF_PRV(Vec3<double>, lower)
    GX_GET_CREF_PRV(Vec3<double>, diameter)
    GX_GET_CREF_PRV(Vec3<double>, center)
    GX_GET_VAL_PRV(double, volume, 0.0)
public:
    Aabb3() noexcept;
    Aabb3(const Vec3<double>& upper, const Vec3<double>& lower) noexcept;
    explicit Aabb3(const Vec3<double>& p) noexcept;
    void update() noexcept;
    void reset() noexcept;
    void reset(const Vec3<double>& p) noexcept;
    void put(const Vec3<double>& p) noexcept;
    void put(const Sphere& o) noexcept;
    void put(const Aabb3& o) noexcept;
    /// For better performance but keep in mind call update after your puts finished
    /// In case your not sure about it, use put, it is safer
    void put_without_update(const Vec3<double>& p) noexcept;
    void put_without_update(const Aabb3& o) noexcept;
    [[nodiscard]] std::optional<double> hit(const math::Ray3& r, double d_min) const noexcept;
    [[nodiscard]] IntersectionStatus check_intersection_status(const Aabb3& o) const noexcept;
    [[nodiscard]] bool check_intersection(const Sphere& o) const noexcept;
    [[nodiscard]] bool check_intersection(const Aabb3& o, Aabb3& intersection) const noexcept;
    [[nodiscard]] bool check_intersection(const Aabb3& o) const noexcept;
    void set_center(const Vec3<double>& c) noexcept;
    void set_diameter(const Vec3<double>& d) noexcept;
    void read(platform::stream::Stream& s) noexcept;
    void get_all_corners(math::Vec3<double> (&corners)[8]) const noexcept;
};
}
#endif
