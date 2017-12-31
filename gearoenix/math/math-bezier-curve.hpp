#ifndef GEAROEMIX_MATH_BEZIER_CURVE_HPP
#define GEAROEMIX_MATH_BEZIER_CURVE_HPP
#include "math-vector.hpp"
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace math {
    struct Vec2;
    struct Vec3;
    struct Vec4;
    struct CubicBezierCurve3D {
    public:
        struct Point {
            math::Vec3 position;
            math::Vec3 in;
            math::Vec3 out;
        };

    private:
        std::vector<Point> points;

    public:
        CubicBezierCurve3D();
        CubicBezierCurve3D(int points_count);
        void set_point(int index, const Point& p);
        void render(std::uint32_t* pixels, const int img_width, const int img_height, const std::uint32_t color = 0XFFFFFFFF);
    };
}
}
#endif
