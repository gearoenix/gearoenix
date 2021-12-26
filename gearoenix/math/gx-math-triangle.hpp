#ifndef GEAROENIX_MATH_TRIANGLE_HPP
#define GEAROENIX_MATH_TRIANGLE_HPP
#include "gx-math-vector-3d.hpp"

namespace gearoenix::math {
template <typename Element>
struct Ray3;

template <typename Element>
struct Triangle3 {
private:
    Vec3<Element> head;
    Vec3<Element> edge[2];

public:
    constexpr Triangle3() noexcept = default;

    constexpr Triangle3(const Vec3<Element>& p1, const Vec3<Element>& p2, const Vec3<Element>& p3)
        : head(p1)
    {
        edge[0] = p2 - p1;
        edge[1] = p3 - p1;
    }

    constexpr bool intersect(const Ray3<Element>& r, const Element tmin, Vec3<Element>& out_factors) const
    {
        const auto pvec = r.get_normalized_direction().cross(edge[1]);
        const auto det = edge[0].dot(pvec);
        if (static_cast<Element>(-0.001) < det && det < static_cast<Element>(0.001)) {
            return false;
        }
        const auto inv_det = 1.0f / det;
        const auto tvec = r.get_origin() - head;
        const auto u = tvec.dot(pvec) * inv_det;
        if (u < static_cast<Element>(0) || u > static_cast<Element>(1)) {
            return false;
        }
        const auto qvec = tvec.cross(edge[0]);
        const auto v = r.get_normalized_direction().dot(qvec) * inv_det;
        if (v < static_cast<Element>(0) || u + v > static_cast<Element>(1)) {
            return false;
        }
        const auto t = edge[1].dot(qvec) * inv_det; // Set distance along ray to intersection
        if (t < tmin) {
            if (t > static_cast<Element>(0)) {
                out_factors.x = t;
                out_factors.y = u;
                out_factors.z = v;
                return true;
            }
        }
        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const Triangle3& t) noexcept
    {
        os << R"("Triangle3": { "head": ")" << t.head << R"(", "edge0": ")" << t.edge[0] << R"(", "edge1": ")" << t.edge[1] << "\" }\n";
        return os;
    }
};
}
#endif
