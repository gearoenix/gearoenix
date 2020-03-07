#ifndef GEAROENIX_MATH_BEZIER_CURVE_HPP
#define GEAROENIX_MATH_BEZIER_CURVE_HPP
#include "math-vector-2d.hpp"
#include <vector>

namespace gearoenix::math {
class CubicBezierCurve2D {
public:
    struct Point {
        math::Vec2<double> position;
        math::Vec2<double> in;
        math::Vec2<double> out;
    };

private:
    std::vector<Point> points;
    void create_smooth_non_overlapping_blunt_closed();

public:
    CubicBezierCurve2D();
    explicit CubicBezierCurve2D(int points_count);
    CubicBezierCurve2D(int points_count, bool smooth, bool overlapable, bool fast_curvable, bool closed);
    void normalize(double scale = 1.0);
    void set_point(int index, const Point& p);
    void render(std::uint32_t* pixels, int img_width, int img_height, std::uint32_t color = 0XFFFFFFFF);
};
}
#endif
