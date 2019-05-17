#ifndef GEAROENIX_MATH_RAY_HPP
#define GEAROENIX_MATH_RAY_HPP
#include "math-vector.hpp"
namespace gearoenix {
namespace math {
    struct Triangle3;
    struct Ray3 {
        friend struct Triangle3;

    private:
        Vec3 o;
        Vec3 d; // this must be normal
    public:
        Ray3(const Vec3& origin, const Vec3& normalized_direction);
        const Vec3& get_origin() const;
        const Vec3& get_direction() const;
    };
}
}
#endif
