#ifndef GEAROEMIX_MATH_PLOTTER_HPP
#define GEAROEMIX_MATH_PLOTTER_HPP
#include "math-vector.hpp"
#include <cstdint>
#include <tuple>
#include <vector>
namespace gearoenix {
namespace math {
    class Plotter {
    public:
        class Brush {
            friend class Plotter;

        private:
            const std::uint32_t color;
            std::vector<std::tuple<int, int>> indices = { std::make_tuple(0, 0) };

        public:
            Brush(const int thickness = 0, const std::uint32_t color = 0XFFFFFFFF);
        };

    private:
        // this is not owner of pointer
        std::uint32_t* const pixels;

        const int img_width, img_height;
        void plot(const int x, const int y, const Brush& b = Brush());

    public:
        Plotter(std::uint32_t* const pixels, const int img_width, const int img_height);
        void draw_point(const Vec2& pos, const Brush& b = Brush());
        void draw_line(const Vec2& start, const Vec2& end, const Brush& b = Brush());
    };
}
}
#endif
