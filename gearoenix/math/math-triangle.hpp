#ifndef GEAROENIX_MATH_TRIANGLE_HPP
#define GEAROENIX_MATH_TRIANGLE_HPP
#include "math-vector-3d.hpp"

namespace gearoenix::math {
struct Ray3;
struct Triangle3 {
private:
    Vec3<double> head;
    Vec3<double> edge[2];

public:
    static const double epsilon;
    Triangle3() = default;
    Triangle3(const Vec3<double>& p1, const Vec3<double>& p2, const Vec3<double>& p3);
    bool intersect(const Ray3& r, double tmin, Vec3<double>& out_factors) const;

    friend std::ostream& operator<<(std::ostream& os, const Triangle3& t) noexcept
    {
        os << R"("Triangle3": { "head": ")" << t.head << R"(", "edge0": ")" << t.edge[0] << R"(", "edge1": ")" << t.edge[1] << "\" }\n";
        return os;
    }
};
}
#endif
