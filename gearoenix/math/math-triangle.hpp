#ifndef GEAROEMIX_MATH_TRIANGLE_HPP
#define GEAROEMIX_MATH_TRIANGLE_HPP
#include "math-ray.hpp"
#include "math-vector.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace math {
    struct Triangle3 {
    private:
        Vec3 head;
        Vec3 edge[2];

    public:
        static const core::Real epsilon;
        bool intersect(const Ray3& r, const core::Real tmin, Vec3& out_factors) const;
    };
}
}
#endif
