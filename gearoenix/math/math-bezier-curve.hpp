#ifndef GEAROENIX_MATH_BEZIER_CURVE_HPP
#define GEAROENIX_MATH_BEZIER_CURVE_HPP
#include "math-vector.hpp"
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace math {
    class CubicBezierCurve2D {
    public:
        struct Point {
            math::Vec2 position;
            math::Vec2 in;
            math::Vec2 out;
        };

    private:
        std::vector<Point> points;
        void create_smooth_nonoverlaping_blunt_closed();

    public:
        CubicBezierCurve2D();
        CubicBezierCurve2D(const int points_count);
        CubicBezierCurve2D(const int points_count, const bool smooth, const bool overlapable, const bool fast_curvable, const bool closed);
        void normalize(const core::Real scale = 1.0f);
        void set_point(const int index, const Point& p);
        void render(std::uint32_t* pixels, const int img_width, const int img_height, const std::uint32_t color = 0XFFFFFFFF);
    };
}
}
#endif
