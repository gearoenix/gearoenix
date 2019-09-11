#ifndef GEAROENIX_RENDER_WIDGET_ALIGNMENT_HPP
#define GEAROENIX_RENDER_WIDGET_ALIGNMENT_HPP
#include <cstdint>
namespace gearoenix::render::widget {
enum struct Alignment : std::uint8_t {
    CenterBottom = 1,
    CenterMiddle = 2,
    CenterTop = 3,
    LeftBottom = 4,
    LeftMiddle = 5,
    LeftTop = 6,
    RightBottom = 7,
    RightMiddle = 8,
    RightTop = 9,
};
}
#endif