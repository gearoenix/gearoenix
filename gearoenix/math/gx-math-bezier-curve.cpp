#include "gx-math-bezier-curve.hpp"
#include "gx-math-plotter.hpp"
#include <cmath>
#include <random>

void gearoenix::math::CubicBezierCurve2D::create_smooth_non_overlapping_blunt_closed()
{
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_real_distribution ud1(static_cast<core::fp_t>(0.8), static_cast<core::fp_t>(1.3));
    std::uniform_real_distribution ud2(static_cast<core::fp_t>(0.6), static_cast<core::fp_t>(1.3));
    const auto cnt = static_cast<int>(points.size() - 1);
    const auto d = static_cast<core::fp_t>(3.14) / static_cast<core::fp_t>(cnt);
    const auto d2 = static_cast<core::fp_t>(2.0) * d;
    const auto cl = std::tan(d);
    const auto sin = std::sin(d2);
    const auto cos = std::cos(d2);
    const Vec2 rot_mr1(cos, -sin);
    const Vec2 rot_mr2(sin, cos);
    Vec2 las_pos(static_cast<core::fp_t>(1), static_cast<core::fp_t>(0));
    Vec2 las_crt(static_cast<core::fp_t>(0), static_cast<core::fp_t>(1));
    for (int i = 0; i < cnt; ++i) {
        Point& p = points[i];
        p.position = las_pos * ud2(e1);
        p.out = p.position + (las_crt * (cl * ud1(e1)));
        p.in = p.position - (las_crt * (cl * ud1(e1)));
        las_pos = Vec2(rot_mr1.dot(las_pos), rot_mr2.dot(las_pos));
        las_crt = Vec2(rot_mr1.dot(las_crt), rot_mr2.dot(las_crt));
    }
    points[cnt] = points[0];
    for (int pnt_i = 0, pnt_j = 1; pnt_i < cnt; ++pnt_i, ++pnt_j) {
        if (const auto inter = Vec2<core::fp_t>::intersect(points[pnt_i].position, points[pnt_i].out, points[pnt_j].in, points[pnt_j].position)) {
            points[pnt_i].out = *inter;
            if (pnt_i == 0) {
                points[cnt].out = *inter;
            }
            points[pnt_j].in = *inter;
            if (pnt_j == cnt) {
                points[0].in = *inter;
            }
        }
    }
}

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D() = default;

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D(const int points_count)
    : points(points_count)
{
    if constexpr (GX_DEBUG_MODE) {
        if (points_count == 0) {
            GX_UNEXPECTED;
        }
    }
}

gearoenix::math::CubicBezierCurve2D::CubicBezierCurve2D(const int points_count, const bool smooth, const bool overlapable, const bool fast_curvable, const bool closed)
    : points(points_count)
{
    if constexpr (GX_DEBUG_MODE) {
        if (points_count == 0) {
            GX_UNEXPECTED;
        }
    }
    // only 1 case scenario from 16 is needed right now, others are todo when needed
    if (smooth && !overlapable && !fast_curvable && closed) {
        create_smooth_non_overlapping_blunt_closed();
    } else {
        GX_UNIMPLEMENTED;
    }
}

void gearoenix::math::CubicBezierCurve2D::normalize(const core::fp_t scale)
{
    const core::fp_t scale2 = 0.9f * scale;
    const core::fp_t scale3 = 0.05f * scale;
    const int points_count = (int)points.size();
    if (points_count == 0)
        return;
    core::fp_t maxx = points[0].in[0], minx = points[0].in[0];
    core::fp_t maxy = points[0].in[1], miny = points[0].in[1];
    auto mm = [&](const core::fp_t x, const core::fp_t y) {
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
    const core::fp_t s = maxx > maxy ? scale2 / maxx : scale2 / maxy;
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

void gearoenix::math::CubicBezierCurve2D::set_point(const int index, const Point& p) { points[index] = p; }

void gearoenix::math::CubicBezierCurve2D::render(std::uint32_t* pixels, const int img_width, const int img_height, const std::uint32_t color)
{
    if (points.size() == 0)
        return;
    Plotter plotter(pixels, img_width, img_height);
    const Plotter::Brush brush1(10, color);
    const Plotter::Brush brush2(1, 0XFF00FF00);
    const auto cnt = static_cast<int>(points.size() - 1);
    for (int i = 0; i < cnt;) {
        const Vec2<core::fp_t>& p1 = points[i].position;
        const Vec2<core::fp_t>& p2 = points[i].out;
        const Vec2<core::fp_t>& p3 = points[++i].in;
        const Vec2<core::fp_t>& p4 = points[i].position;
        // todo add occlusion culling
        core::fp_t t_step = p1.distance(p2);
        t_step += p2.distance(p3);
        t_step += p3.distance(p4);
        t_step = 1.0f / t_step;
        for (core::fp_t t = t_step; t < 1.0f; t += t_step) {
            const core::fp_t nt = 1.0f - t;
            const core::fp_t nt2 = nt * nt;
            const core::fp_t nt3 = nt2 * nt;
            const core::fp_t t2 = t * t;
            const core::fp_t t3 = t2 * t;
            const core::fp_t ntt23 = nt * t2 * 3.0f;
            const core::fp_t nt2t3 = nt2 * t * 3.0f;
            Vec2 p = p1 * nt3 + p2 * nt2t3 + p3 * ntt23 + p4 * t3;
            plotter.draw_point(p, brush1);
        }
    }
    //---------------------- for debug (it will be removed in near future)
    const int points_count = static_cast<int>(points.size());
    for (int i = 0; i < points_count; ++i) {
        plotter.draw_line(points[i].in, points[i].out, brush2);
    }
    //++++++++++++++++++++++ temporary (it will be removed in near future)
    std::vector<Point> map_points;
    const Plotter::Brush brush3(1, 0XFFFF0000);
    const Plotter::Brush brush4(1, 0XFF0000FF);
    core::fp_t poly_len = 0.0f;
    for (int i = 0; i < cnt;) {
        const Vec2<core::fp_t>& p1 = points[i].position;
        const Vec2<core::fp_t>& p2 = points[i].out;
        const Vec2<core::fp_t>& p3 = points[++i].in;
        const Vec2<core::fp_t>& p4 = points[i].position;
        // todo add occlusion culling
        core::fp_t t_step = p1.distance(p2);
        t_step += p2.distance(p3);
        t_step += p3.distance(p4);
        t_step = 2.0f / t_step;
        Vec2<core::fp_t> prep = p1;
        for (core::fp_t t = t_step; t < 1.0f; t += t_step) {
            const core::fp_t nt = 1.0f - t;
            const core::fp_t nt2 = nt * nt;
            const core::fp_t nt3 = nt2 * nt;
            const core::fp_t t2 = t * t;
            const core::fp_t t3 = t2 * t;
            const core::fp_t ntt23 = nt * t2 * 3.0f;
            const core::fp_t nt2t3 = nt2 * t * 3.0f;
            const Vec2 p = p1 * nt3 + p2 * nt2t3 + p3 * ntt23 + p4 * t3;
            Vec2 prp = p - prep;
            const core::fp_t len = prp.length();
            poly_len += len;
            if (poly_len > 20.0f && len != 0.0f) {
                const core::fp_t i_len = 1.0f / len;
                poly_len = 0.0f;
                prep = p;
                prp *= i_len;
                core::fp_t tmp = prp[0];
                prp[0] = -prp[1];
                prp[1] = tmp;
                Point cur_pnt;
                const Vec2 p_rpt = (prp * 10.0f);
                cur_pnt.in = p + p_rpt;
                cur_pnt.position = p;
                cur_pnt.out = p - p_rpt;
                map_points.push_back(cur_pnt);
            }
        }
    }
    const int prp_count = static_cast<int>(map_points.size());
    Vec2<core::fp_t> pathdir;
    Vec2<core::fp_t> las_pos = map_points[0].position;
    std::vector<Point> m_map_p;
    m_map_p.push_back(map_points[cnt]);
    m_map_p.push_back(map_points[0]);
    pathdir = las_pos - map_points[cnt].position;
    pathdir.normalize();
    for (int i = 1; i < prp_count; ++i) {
        const Vec2<core::fp_t>& p1 = map_points[i].position;
        const Vec2 cur_dir = p1 - las_pos;
        const core::fp_t dv = std::abs(cur_dir.cross(pathdir));
        las_pos = map_points[i].position;
        if (dv > 0.5f) {
            pathdir = cur_dir;
            pathdir.normalize();
            m_map_p.push_back(map_points[i]);
        }
    }
    for (int i = 0; i < static_cast<int>(m_map_p.size()); ++i) {
        plotter.draw_line(m_map_p[i].in, m_map_p[i].position, brush3);
        plotter.draw_line(m_map_p[i].out, m_map_p[i].position, brush4);
    }
}
