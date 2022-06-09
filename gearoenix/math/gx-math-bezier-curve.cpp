#include "gx-math-bezier-curve.hpp"
#include "gx-math-plotter.hpp"
#include <cmath>
#include <random>

void gearoenix::math::CubicBezierCurve2D::create_smooth_non_overlapping_blunt_closed()
{
    // some mine tricks, becareful it has license! :D
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_real_distribution<double> ud1(0.8f, 1.3f);
    std::uniform_real_distribution<double> ud2(0.6f, 1.3f);
    const int cnt = (int)points.size() - 1;
    const double d = 3.14f / double(cnt);
    const double d2 = 2.0f * d;
    const double cl = std::tan(d);
    const double sin = std::sin(d2);
    const double cos = std::cos(d2);
    const Vec2 rotmr1(cos, -sin);
    const Vec2 rotmr2(sin, cos);
    Vec2 laspos(1.0, 0.0);
    Vec2 lascrt(0.0, 1.0);
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
        if (const auto inter = Vec2<double>::intersect(points[pnti].position, points[pnti].out, points[pntj].in, points[pntj].position)) {
            points[pnti].out = *inter;
            if (pnti == 0)
                points[cnt].out = *inter;
            points[pntj].in = *inter;
            if (pntj == cnt)
                points[0].in = *inter;
        }
    }
}

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D() = default;

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D(const int points_count)
    : points(points_count)
{
#ifdef GX_DEBUG_MODE
    if (points_count == 0)
        GX_UNEXPECTED;
#endif
}

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D(const int points_count, const bool smooth, const bool overlapable, const bool fast_curvable, const bool closed)
    : points(points_count)
{
#ifdef GX_DEBUG_MODE
    if (points_count == 0)
        GX_UNEXPECTED;
#endif
    // only 1 case scenario from 16 is needed right now, others are todo when needed
    if (smooth && !overlapable && !fast_curvable && closed)
        create_smooth_non_overlapping_blunt_closed();
    else
        GX_UNIMPLEMENTED;
}

void gearoenix::math::CubicBezierCurve2D::normalize(const double scale)
{
    const double scale2 = 0.9f * scale;
    const double scale3 = 0.05f * scale;
    const int points_count = (int)points.size();
    if (points_count == 0)
        return;
    double maxx = points[0].in[0], minx = points[0].in[0];
    double maxy = points[0].in[1], miny = points[0].in[1];
    auto mm = [&](const double x, const double y) {
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
    const double s = maxx > maxy ? scale2 / maxx : scale2 / maxy;
    for (int i = 0; i < points_count; ++i) {
        Point& p = points[i];
        p.in[0] = (p.in[0] - minx) * s + scale3;
        p.in[1] = (p.in[1] - miny) * s + scale3;
        p.position[0] = (p.position[0] - minx) * s + scale3;
        p.position[1] = (p.position[1] - miny) * s + scale3;
        p.out[0] = (p.out[0] - minx) * s + scale3;
        p.out[1] = (p.out[1] - miny) * s + scale3;
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
    Plotter plotter(pixels, img_width, img_height);
    const Plotter::Brush brush1(10, color);
    const Plotter::Brush brush2(1, 0XFF00FF00);
    const int cnt = (int)points.size() - 1;
    for (int i = 0; i < cnt;) {
        const math::Vec2<double>& p1 = points[i].position;
        const math::Vec2<double>& p2 = points[i].out;
        const math::Vec2<double>& p3 = points[++i].in;
        const math::Vec2<double>& p4 = points[i].position;
        // todo add occlusion culling
        double tstep = p1.distance(p2);
        tstep += p2.distance(p3);
        tstep += p3.distance(p4);
        tstep = 1.0f / tstep;
        for (double t = tstep; t < 1.0f; t += tstep) {
            const double nt = 1.0f - t;
            const double nt2 = nt * nt;
            const double nt3 = nt2 * nt;
            const double t2 = t * t;
            const double t3 = t2 * t;
            const double ntt23 = nt * t2 * 3.0f;
            const double nt2t3 = nt2 * t * 3.0f;
            math::Vec2 p = p1 * nt3 + p2 * nt2t3 + p3 * ntt23 + p4 * t3;
            plotter.draw_point(p, brush1);
        }
    }
    //---------------------- for debug (it will be removed in near future)
    const int points_count = (int)points.size();
    for (int i = 0; i < points_count; ++i) {
        plotter.draw_line(points[i].in, points[i].out, brush2);
    }
    //++++++++++++++++++++++ temporary (it will be removed in near future)
    std::vector<Point> mappoints;
    const Plotter::Brush brush3(1, 0XFFFF0000);
    const Plotter::Brush brush4(1, 0XFF0000FF);
    double polylen = 0.0f;
    for (int i = 0; i < cnt;) {
        const math::Vec2<double>& p1 = points[i].position;
        const math::Vec2<double>& p2 = points[i].out;
        const math::Vec2<double>& p3 = points[++i].in;
        const math::Vec2<double>& p4 = points[i].position;
        // todo add occlusion culling
        double tstep = p1.distance(p2);
        tstep += p2.distance(p3);
        tstep += p3.distance(p4);
        tstep = 2.0f / tstep;
        Vec2<double> prep = p1;
        for (double t = tstep; t < 1.0f; t += tstep) {
            const double nt = 1.0f - t;
            const double nt2 = nt * nt;
            const double nt3 = nt2 * nt;
            const double t2 = t * t;
            const double t3 = t2 * t;
            const double ntt23 = nt * t2 * 3.0f;
            const double nt2t3 = nt2 * t * 3.0f;
            const math::Vec2 p = p1 * nt3 + p2 * nt2t3 + p3 * ntt23 + p4 * t3;
            math::Vec2 prp = p - prep;
            const double len = prp.length();
            polylen += len;
            if (polylen > 20.0f && len != 0.0f) {
                const double ilen = 1.0f / len;
                polylen = 0.0f;
                prep = p;
                prp *= ilen;
                double tmp = prp[0];
                prp[0] = -prp[1];
                prp[1] = tmp;
                Point curpnt;
                const Vec2 prpt = (prp * 10.0f);
                curpnt.in = p + prpt;
                curpnt.position = p;
                curpnt.out = p - prpt;
                mappoints.push_back(curpnt);
            }
        }
    }
    const int prp_count = (int)mappoints.size();
    Vec2<double> pathdir;
    Vec2<double> laspos = mappoints[0].position;
    std::vector<Point> mmapp;
    mmapp.push_back(mappoints[cnt]);
    mmapp.push_back(mappoints[0]);
    pathdir = laspos - mappoints[cnt].position;
    pathdir.normalize();
    for (int i = 1; i < prp_count; ++i) {
        const Vec2<double>& p1 = mappoints[i].position;
        const Vec2 curdir = p1 - laspos;
        const double dv = std::abs(curdir.cross(pathdir));
        laspos = mappoints[i].position;
        if (dv > 0.5f) {
            pathdir = curdir;
            pathdir.normalize();
            mmapp.push_back(mappoints[i]);
        }
    }
    for (int i = 0; i < (int)mmapp.size(); ++i) {
        plotter.draw_line(mmapp[i].in, mmapp[i].position, brush3);
        plotter.draw_line(mmapp[i].out, mmapp[i].position, brush4);
    }
}
