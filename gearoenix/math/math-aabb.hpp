#ifndef GEAROEMIX_MATH_AABB_HPP
#define GEAROEMIX_MATH_AABB_HPP
#include "math-vector.hpp"
#include "math-intersection-status.hpp"
namespace gearoenix {
namespace math {
    struct Ray3;
    struct Aabb3 {
        Vec3 mx;
        Vec3 mn;
        explicit Aabb3() noexcept;
		explicit Aabb3(const Vec3& mx, const Vec3& mn) noexcept;
		void reset() noexcept;
		void reset(const Vec3& p) noexcept;
        void put(const Vec3& p) noexcept;
        bool test(const Ray3& r, core::Real& tmin) const noexcept;
        bool test(const Aabb3& o, Aabb3& intersection) const noexcept;
		bool test(const Aabb3& o) const noexcept;
		IntersectionStatus::Type check_intersection(const Aabb3 &o) const noexcept;
    };
}
}
#endif
