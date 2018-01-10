#ifndef GEAROEMIX_MATH_AABB_HPP
#define GEAROEMIX_MATH_AABB_HPP
#include "math-vector.hpp"
namespace gearoenix {
namespace math {
    struct Ray3;
    struct Aabb3 {
    private:
        Vec3 a; // max
        Vec3 b; // min
    public:
        Aabb3();
        void reset(const Vec3& p);
        void put(const Vec3& p);
        bool test(const Ray3& r, core::Real& tmin);
    };
}
}
#endif
