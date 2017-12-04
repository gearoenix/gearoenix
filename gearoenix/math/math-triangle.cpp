#include "math-triangle.hpp"

const gearoenix::core::Real gearoenix::math::Triangle3::epsilon = 0.001f;

bool gearoenix::math::Triangle3::intersect(const Ray3& r, const core::Real tmin, Vec3& out_factors) const
{
    const Vec3 pvec = r.d.cross(edge[1]);
    const core::Real det = edge[0].dot(pvec);
    if (det < epsilon && det > -epsilon) {
        return false;
    }
    const core::Real inv_det = 1.0f / det;
    const Vec3 tvec = r.o - head;
    const core::Real u = tvec.dot(pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    const Vec3 qvec = tvec.cross(edge[0]);
    const core::Real v = r.d.dot(qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    const core::Real t = edge[1].dot(qvec) * inv_det; // Set distance along ray to intersection
    if (t < tmin) {
        if (t > epsilon) {
            out_factors[0] = t;
            out_factors[1] = u;
            out_factors[2] = v;
            return true;
        }
    }
    return false;
}
