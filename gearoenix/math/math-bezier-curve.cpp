#include "math-bezier-curve.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../system/sys-log.hpp"
#include <cmath>
#include <random>

void gearoenix::math::CubicBezierCurve2D::create_smooth_nonoverlaping_blunt(const int points_count, bool closed)
{
    const int cnt = closed ? points_count - 1 : points_count;
    const core::Real d = 3.14f / core::Real(cnt);
    const core::Real d2 = 2.0f * d;
    const core::Real cl = std::tan(d);
    for (int i = 0; i < cnt; ++i) {
    }
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_real_distribution<core::Real> ud1(-1.0f, 0.3f);
    std::uniform_real_distribution<core::Real> ud2(0.3f, 1.0f);
    std::uniform_real_distribution<core::Real> ud3(-1.36f, 1.36f);
    Vec2 v;
    Vec2 lp;
    {
        const Vec2 v1(ud1(e1) * ud2(e1), ud1(e1) * ud2(e1));
        v = v1.normalized();
        const Vec2 p1(ud1(e1) * ud2(e1), ud1(e1) * ud2(e1));
        const Vec2 p2 = p1 + v1;
        const Vec2 p3 = p2 + (v * ud2(e1));
        points[0].in = p1;
        points[0].position = p2;
        points[0].out = p3;
        lp = p3;
    }
    auto randv = [&]() {
        const core::Real th = 3.14f + ud3(e1);
        const core::Real sin = std::sin(th);
        const core::Real cos = std::cos(th);
        v = Vec2(v[0] * cos - v[1] * sin, v[1] * sin + v[1] * cos);
    };
    for (int i = 1; i < cnt; ++i) {
        randv();
        const Vec2 p1 = lp + v * (ud2(e1) + 0.3f);
        randv();
        const Vec2 p2 = p1 + v * ud2(e1);
        const Vec2 p3 = p2 + v * ud2(e1);
        points[i].in = p1;
        points[i].position = p2;
        points[i].out = p3;
        lp = p3;
    }
    if (closed) {
        points[cnt] = points[0];
    }
}

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D()
{
}

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D(const int points_count)
    : points(points_count)
{
#ifdef DEBUG_MODE
    if (points_count == 0)
        UNEXPECTED;
#endif
}

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D(const int points_count, const bool smooth, const bool overlapable, const bool fast_curvable, const bool closed)
    : points(points_count)
{
#ifdef DEBUG_MODE
    if (points_count == 0)
        UNEXPECTED;
#endif
    // only 2 case scenarios from 16 are needed right now, others are todo when needed
    if (!smooth)
        UNIMPLEMENTED;
    if (overlapable)
        UNIMPLEMENTED;
    if (fast_curvable)
        UNIMPLEMENTED;
    create_smooth_nonoverlaping_blunt(points_count, closed);
}

void gearoenix::math::CubicBezierCurve2D::normalize()
{
    const int points_count = points.size();
    if (points_count == 0)
        return;
    core::Real maxx = points[0].in[0], minx = points[0].in[0];
    core::Real maxy = points[0].in[1], miny = points[0].in[1];
    auto mm = [&](const core::Real x, const core::Real y) {
        if (maxx < x)
            maxx = x;
        else if (minx > x)
            minx = x;
        if (maxy < y)
            maxy = y;
        else if (miny > y)
            miny = y;
    };
    for (const Point& p : points) {
        mm(p.in[0], p.in[1]);
        mm(p.position[0], p.position[1]);
        mm(p.out[0], p.out[1]);
    }
    maxx -= minx;
    maxy -= miny;
    if (maxx == 0.0f || maxy == 0.0f)
        return;
    const core::Real s = maxx > maxy ? 1.9f / maxx : 1.9f / maxy;
    for (int i = 0; i < points_count; ++i) {
        Point& p = points[i];
        p.in[0] = (p.in[0] - minx) * s - 0.95f;
        p.in[1] = (p.in[1] - miny) * s - 0.95f;
        p.position[0] = (p.position[0] - minx) * s - 0.95f;
        p.position[1] = (p.position[1] - miny) * s - 0.95f;
        p.out[0] = (p.out[0] - minx) * s - 0.95f;
        p.out[1] = (p.out[1] - miny) * s - 0.95f;
    }
}

void gearoenix::math::CubicBezierCurve2D::set_point(const int index, const Point& p)
{
    points[index] = p;
}

void gearoenix::math::CubicBezierCurve2D::render(std::uint32_t* pixels, const int img_width, const int img_height, const std::uint32_t color)
{
    if (points.size() == 0)
        return;
    const int cw = img_width / 2;
    const int ch = img_height / 2;
    const core::Real cwf = core::Real(cw);
    const core::Real chf = core::Real(ch);
    const core::Real coef = chf * cwf;
    const int cnt = points.size() - 1;
    for (int i = 0; i < cnt;) {
        const math::Vec2& p1 = points[i].position;
        const math::Vec2& p2 = points[i].out;
        const math::Vec2& p3 = points[++i].in;
        const math::Vec2& p4 = points[i].position;
        // todo add occlusion culling
        core::Real tstep = p1.distance(p2);
        tstep += p2.distance(p3);
        tstep += p3.distance(p4);
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
