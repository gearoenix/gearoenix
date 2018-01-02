#include "math-bezier-curve.hpp"
#include "../core/cr-build-configuration.hpp"
#include "../system/sys-log.hpp"
#include <cmath>
#include <random>

void gearoenix::math::CubicBezierCurve2D::create_smooth_nonoverlaping_blunt_closed()
{
    // some mine tricks, becareful it has license! :D
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_real_distribution<core::Real> ud1(0.8f, 1.3f);
    std::uniform_real_distribution<core::Real> ud2(0.6f, 1.3f);
    const int cnt = points.size() - 1;
    const core::Real d = 3.14f / core::Real(cnt);
    const core::Real d2 = 2.0f * d;
    const core::Real cl = std::tan(d);
    const core::Real sin = std::sin(d2);
    const core::Real cos = std::cos(d2);
    const Vec2 rotmr1(cos, -sin);
    const Vec2 rotmr2(sin, cos);
    Vec2 laspos(1.0f, 0.0f);
    Vec2 lascrt(0.0f, 1.0f);
    for (int i = 0; i < cnt; ++i) {
        Point& p = points[i];
        p.position = laspos * ud2(e1);
        p.out = p.position + (lascrt * (cl * ud1(e1)));
        p.in = p.position - (lascrt * (cl * ud1(e1)));
        laspos = Vec2(rotmr1.dot(laspos), rotmr2.dot(laspos));
        lascrt = Vec2(rotmr1.dot(lascrt), rotmr2.dot(lascrt));
    }
    points[cnt] = points[0];
    for (int pnti = 0, pntj = 1; pnti < cnt; ++pnti, ++pntj) {
        Vec2 inter;
        if (Vec2::intersect(points[pnti].position, points[pnti].out, points[pntj].in, points[pntj].position, inter)) {
            points[pnti].out = inter;
            if (pnti == 0)
                points[cnt].out = inter;
            points[pntj].in = inter;
            if (pntj == cnt)
                points[0].in = inter;
        }
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
    // only 1 case scenario from 16 is needed right now, others are todo when needed
    if (smooth && !overlapable && !fast_curvable && closed)
        create_smooth_nonoverlaping_blunt_closed();
    else
        UNIMPLEMENTED;
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
    // todo add framized rendering instead of whole scaling
    // todo: it is not very optimized, its speed is not that bad, but in future optimize it.
    if (points.size() == 0)
        return;
    const int cw = img_width / 2;
    const int ch = img_height / 2;
    const core::Real cwf = core::Real(cw);
    const core::Real chf = core::Real(ch);
    const core::Real coef = chf * cwf;
    const int cnt = points.size() - 1;
    const std::uint32_t axis_color = 0xFF80F04F;
    core::Real polylen = 0.0f;
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
        Vec2 prep = p1;
        for (core::Real t = tstep; t <= 1.0f; t += tstep) {
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
            math::Vec2 prp = p - prep;
            polylen += prp.length();
            bool is_poly = false;
            if (polylen > 0.02) {
                if (polylen > 0.03) {
                    polylen = 0.0f;
                }
                is_poly = true;
            }
            prep = p;
            prp.normalize();
            core::Real tmp = prp[0];
            prp[0] = -prp[1];
            prp[1] = tmp;
            math::Vec2 prpt = prp;
            for (int pi = 0; pi < 20; ++pi, prpt += prp) {
                {
                    const int ppw = int(prpt[0]) + pw;
                    const int pph = int(prpt[1]) + ph;
                    pixels[pph * img_height + ppw] = is_poly ? 0xffff0000 : color;
                }
                {
                    const int ppw = pw - int(prpt[0]);
                    const int pph = ph - int(prpt[1]);
                    pixels[pph * img_height + ppw] = is_poly ? 0xff0000ff : color;
                }
            }
        }
    }
    for (int i = 0; i < cnt;) {
        const math::Vec2& p1 = points[i].position;
        const math::Vec2& p2 = points[i].out;
        const math::Vec2& p3 = points[++i].in;
        const math::Vec2& p4 = points[i].position;
        core::Real tstep = p1.distance(p2);
        tstep += p2.distance(p3);
        tstep += p3.distance(p4);
        tstep *= coef;
        tstep = 1.0f / tstep;
        //        Vec2 prep = p1;
        for (core::Real t = tstep; t <= 1.0f; t += tstep) {
            const core::Real nt = 1.0f - t;
            //            const core::Real nt2 = nt * nt;
            //            const core::Real nt3 = nt2 * nt;
            //            const core::Real t2 = t * t;
            //            const core::Real t3 = t2 * t;
            //            const core::Real ntt23 = nt * t2 * 3.0f;
            //            const core::Real nt2t3 = nt2 * t * 3.0f;
            //            math::Vec2 p = p1 * nt3 + p2 * nt2t3 + p3 * ntt23 + p4 * t3;
            //            if (p[0] < -1.0f || p[0] > 1.0f || p[1] < -1.0f || p[1] > 1.0f)
            //                continue;
            //            const int pw = int(cwf * p[0]) + cw;
            //            const int ph = int(chf * p[1]) + ch;
            //            pixels[ph * img_height + pw] = color;
            //            math::Vec2 prp = p - prep;
            //            prep = p;
            //            prp.normalize();
            //            core::Real tmp = prp[0];
            //            prp[0] = -prp[1];
            //            prp[1] = tmp;
            //            math::Vec2 prpt = prp;
            //            for(int pi = 0; pi < 10; ++pi, prpt += prp)
            //            {
            //                {
            //                    const int ppw = int(prpt[0]) + pw;
            //                    const int pph = int(prpt[1]) + ph;
            //                    pixels[pph * img_height + ppw] = 0XFFFF0000;
            //                }
            //                {
            //                    const int ppw = pw - int(prpt[0]);
            //                    const int pph = ph - int(prpt[1]);
            //                    pixels[pph * img_height + ppw] = 0XFFFF0000;
            //                }
            //            }
            /////////// temporary
            math::Vec2 p = p1 * nt + p2 * t;
            if (p[0] < -1.0f || p[0] > 1.0f || p[1] < -1.0f || p[1] > 1.0f)
                continue;
            int tpw = int(cwf * p[0]) + cw;
            int tph = int(chf * p[1]) + ch;
            pixels[tph * img_height + tpw] = axis_color;
            p = p3 * nt + p4 * t;
            if (p[0] < -1.0f || p[0] > 1.0f || p[1] < -1.0f || p[1] > 1.0f)
                continue;
            tpw = int(cwf * p[0]) + cw;
            tph = int(chf * p[1]) + ch;
            pixels[tph * img_height + tpw] = axis_color;
            ////////// temporary
        }
    }
}
