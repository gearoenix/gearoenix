#ifndef GEAROENIX_MATH_PLOTTER_HPP
#define GEAROENIX_MATH_PLOTTER_HPP
#include "math-vector-2d.hpp"
#include <cstdint>
#include <tuple>
#include <vector>

namespace gearoenix::math {
class Plotter {
public:
    class Brush {
        friend class Plotter;

    private:
        const std::uint32_t color;
        std::vector<std::tuple<int, int>> indices;

    public:
        explicit Brush(int thickness = 1, std::uint32_t color = 0XFFFFFFFF);
    };

private:
    // this is not owner of pointer
    std::uint32_t* const pixels;

    const int img_width, img_height;
    void plot(int x, int y, const Brush& b = Brush());

public:
    Plotter(std::uint32_t* pixels, int img_width, int img_height);
    void draw_point(const Vec2<double>& pos, const Brush& b = Brush());
    void draw_line(const Vec2<double>& start, const Vec2<double>& end, const Brush& b = Brush());
};
}
#endif
