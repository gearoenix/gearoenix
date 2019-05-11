#ifndef GEAROEMIX_MATH_AABB_HPP
#define GEAROEMIX_MATH_AABB_HPP
#include "math-vector.hpp"
namespace gearoenix {
namespace math {
    struct Ray3;
    struct Aabb3 {
        Vec3 mx;
        Vec3 mn;
        explicit Aabb3() noexcept;
		explicit Aabb3(const Vec3& mx, const Vec3& mn) noexcept;
		void reset();
		void reset(const Vec3& p);
        void put(const Vec3& p);
        bool test(const Ray3& r, core::Real& tmin) const;
        bool test(const Aabb3& o, Aabb3& intersection) const;
    };
}
}
#endif
