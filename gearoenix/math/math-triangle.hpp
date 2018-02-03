#ifndef GEAROEMIX_MATH_TRIANGLE_HPP
#define GEAROEMIX_MATH_TRIANGLE_HPP
#include "math-vector.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace math {
    struct Ray3;
    struct Triangle3 {
    private:
        Vec3 head;
        Vec3 edge[2];

    public:
        static const core::Real epsilon;
        Triangle3();
        Triangle3(const Vec3& p1, const Vec3& p2, const Vec3& p3);
        bool intersect(const Ray3& r, const core::Real tmin, Vec3& out_factors) const;
        void print() const;
    };
}
}
#endif
