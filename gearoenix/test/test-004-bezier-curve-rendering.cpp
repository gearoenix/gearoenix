#include "test-004-bezier-curve-rendering.hpp"
#ifdef TEST004
#include "../math/math-bezier-curve.hpp"
#include "../render/texture/rnd-txt-lodepng.hpp"

int main()
{
    for (int imgi = 0; imgi < 100; ++imgi) {
        gearoenix::math::CubicBezierCurve2D curve(10, true, false, false, true);
        curve.normalize();
        std::vector<unsigned char> pixels(1024 * 1024 * 4);
        std::uint64_t* data = (std::uint64_t*)pixels.data();
        for (int i = 0; i < 1024 * 512; ++i)
            data[i] = 0xFF000000FF000000;
        curve.render((std::uint32_t*)data, 1024, 1024);
        lodepng::encode(std::string("out") + std::to_string(imgi) + ".png", pixels, 1024, 1024);
    }
    return 0;
}

#endif
