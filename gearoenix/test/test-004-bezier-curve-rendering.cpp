#include "test-004-bezier-curve-rendering.hpp"
#ifdef TEST004
#include "../math/math-bezier-curve.hpp"
#include <random>

int main()
{
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_real_distribution<gearoenix::core::Real> ud(-1.0f, 1.0f);
    const int points_count = 5;
    gearoenix::math::CubicBezierCurve3D curve(points_count);
    gearoenix::math::CubicBezierCurve3D::Point p;
    for (int i = 0; i < points_count; ++i) {
        p.position[0] = ud(e1);
        p.position[1] = ud(e1);
        p.in[0] = ud(e1);
        p.in[1] = ud(e1);
        p.out[0] = ud(e1);
        p.out[1] = ud(e1);
        curve.set_point(i, p);
    }
    return 0;
}

#endif
