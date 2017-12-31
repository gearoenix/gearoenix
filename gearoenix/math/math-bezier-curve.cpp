#include "math-bezier-curve.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../system/sys-log.hpp"

gearoenix::math::CubicBezierCurve3D::CubicBezierCurve3D()
{
}

gearoenix::math::CubicBezierCurve3D::CubicBezierCurve3D(int points_count)
    : points(points_count)
{
}

void gearoenix::math::CubicBezierCurve3D::set_point(int index, const Point& p)
{
    points[index] = p;
}

void gearoenix::math::CubicBezierCurve3D::render(std::uint32_t* pixels, const int img_width, const int img_height, const std::uint32_t color)
{
#ifdef DEBUG_MODE
    if (points.size() == 0)
        UNEXPECTED;
#endif
    const int cw = img_width / 2;
    const int ch = img_height / 2;
    const core::Real cwf = core::Real(cw);
    const core::Real chf = core::Real(ch);
    const core::Real coef = chf * cwf;
    const int cnt = points.size() - 1;
    for (int i = 0; i < cnt;) {
        const math::Vec2 p1 = points[i].position.xy();
        const math::Vec2 p2 = points[i].out.xy();
        const math::Vec2 p3 = points[++i].in.xy();
        const math::Vec2 p4 = points[i].position.xy();
        // todo add occlusion culling
        core::Real tstep = p1.square_distance(p2);
        tstep += p2.square_distance(p3);
        tstep += p3.square_distance(p4);
        tstep *= coef;
        tstep = 1.0f / tstep;
        for (core::Real t = 0.0f; t < 1.0; t += tstep) {
            const core::Real nt = 1.0f - t;
            const core::Real nt2 = nt * nt;
            const core::Real nt3 = nt2 * nt;
            const core::Real t2 = t * t;
            const core::Real t3 = t2 * t;
            const core::Real ntt23 = nt * t2 * 3.0f;
            const core::Real nt2t3 = nt2 * t * 3.0f;
            math::Vec2 p = p1 * nt3 + p2 * nt2t3 + p3 * ntt23 + p4 * t3;
            if (p[0] < -1.0f || p[0] > 1.0f || p[1] < -1.0f || p[1] > 1.0f)
                continue;
            const int pw = int(cwf * p[0]) + cw;
            const int ph = int(chf * p[1]) + ch;
            pixels[ph * img_height + pw] = color;
        }
    }
}
