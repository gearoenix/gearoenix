#ifndef GEAROENIX_MATH_TRIANGLE_HPP
#define GEAROENIX_MATH_TRIANGLE_HPP
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
        Triangle3() = default;
        Triangle3(const Vec3& p1, const Vec3& p2, const Vec3& p3);
        bool intersect(const Ray3& r, core::Real tmin, Vec3& out_factors) const;

        friend std::ostream& operator<<(std::ostream& os, const Triangle3& t) noexcept
        {
            os << "Triangle3 { head: " << t.head << ", edge[0]:" << t.edge[0] << ", edge[1]: " << t.edge[1] << "}";
            return os;
        }
    };
}
}
#endif
