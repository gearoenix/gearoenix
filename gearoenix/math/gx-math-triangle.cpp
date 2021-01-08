#include "gx-math-triangle.hpp"
#include "gx-math-ray.hpp"

gearoenix::math::Triangle3::Triangle3(const Vec3<double>& p1, const Vec3<double>& p2, const Vec3<double>& p3)
    : head(p1)
{
    edge[0] = p2 - p1;
    edge[1] = p3 - p1;
}

bool gearoenix::math::Triangle3::intersect(const Ray3& r, const double tmin, Vec3<double>& out_factors) const
{
    const Vec3 pvec = r.get_normalized_direction().cross(edge[1]);
    const double det = edge[0].dot(pvec);
    if (-0.001 < det && det < 0.001) {
        return false;
    }
    const double inv_det = 1.0f / det;
    const Vec3 tvec = r.get_origin() - head;
    const double u = tvec.dot(pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    const Vec3 qvec = tvec.cross(edge[0]);
    const double v = r.get_normalized_direction().dot(qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    const double t = edge[1].dot(qvec) * inv_det; // Set distance along ray to intersection
    if (t < tmin) {
        if (t > 0.0f) {
            out_factors.x = t;
            out_factors.y = u;
            out_factors.z = v;
            return true;
        }
    }
    return false;
}
